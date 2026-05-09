import { copyFileSync, existsSync, readFileSync, writeFileSync } from "fs";
import { basename } from "path";

const files = new Bun.Glob("assets/exported/**/*.obj").scanSync();

const outDir = "source/generated";
const outHeaderPath = `${outDir}/exported.h`;
const outSourcePath = `${outDir}/exported.c`;

let headerDeclarations = "";
let sourceDefinitions = "";

let allObjData: ObjData[] = [];
let allMarkerData: MarkerData[] = [];
let allTextureData: TextureData[] = [];

interface TextureData {
  path: string;
  variableName: string;
  includePath: string;
}

interface MarkerData {
  name: string;
  position: number[];
}

interface ObjData {
  name: string;
  vertices: number[][];
  uvs: number[][];
  faces: number[][];
  normals: number[][];
  material: string | null;
}

function formatName(name: string): string {
  return name
    .replace(".", "_")
    .replace(/[^a-zA-Z0-9_]/g, "_")
    .toUpperCase();
}

function formatTextureVariable(path: string): string {
  const baseName = basename(path).replaceAll(".", "_").replaceAll(" ", "");
  return baseName;
}

function formatTextureInclude(path: string): string {
  const baseName = basename(path).replaceAll(".", "_");
  return baseName + ".h";
}

for (const file of files) {
  const content = readFileSync(file, "utf-8");

  const formattedName = formatName(basename(file, ".obj"));

  const lines = content.split("\n");

  const objData: ObjData = {
    name: formattedName,
    vertices: [],
    uvs: [],
    faces: [],
    normals: [],
    material: null,
  };

  // Parse the OBJ file
  for (const line of lines) {
    const [start, ...rest] = line.trim().split(/\s+/);
    if (!start) continue;
    if (rest.length === 0) continue;

    if (start === "o") {
      //objData.name = formatName(rest[0]!);
    } else if (start === "v") {
      objData.vertices.push(rest.map(Number));
    } else if (start === "vt") {
      objData.uvs.push(rest.map(Number));
    } else if (start === "f") {
      objData.faces.push(
        rest
          .map((face) => {
            const parts = face.split("/");
            return [Number(parts[0]) - 1, parts[1] ? Number(parts[1]) - 1 : -1];
          })
          .flat(),
      );
    } else if (start === "usemtl") {
      objData.material = rest.join(" ");
    }
  }

  // Create the final flat array of [x, y, z, u, v]
  // with simple fan triangulation for convex polygons (like quads)
  const finalVertices: number[][] = [];
  for (const face of objData.faces) {
    const numVerts = face.length / 2;
    for (let i = 1; i < numVerts - 1; i++) {
      // Vertex 0 (pivot)
      const v0 = objData.vertices[face[0]!] || [0, 0, 0];
      const uv0 = objData.uvs[face[1]!] || [0, 0];
      finalVertices.push([...v0, uv0[0]!, 1.0 - uv0[1]!]);

      // Vertex i
      const vi = objData.vertices[face[i * 2]!] || [0, 0, 0];
      const uvi = objData.uvs[face[i * 2 + 1]!] || [0, 0];
      finalVertices.push([...vi, uvi[0]!, 1.0 - uvi[1]!]);

      // Vertex i+1
      const vi1 = objData.vertices[face[(i + 1) * 2]!] || [0, 0, 0];
      const uvi1 = objData.uvs[face[(i + 1) * 2 + 1]!] || [0, 0];
      finalVertices.push([...vi1, uvi1[0]!, 1.0 - uvi1[1]!]);
    }
  }

  // Check if marker
  const isMarker = objData.name.includes("MARKER");

  if (isMarker) {
    // Get the position of the marker (average of all vertices)
    // Marker is originally a cube, all verticies lead to the center point
    const position = [0, 0, 0];
    for (const vertex of objData.vertices) {
      position[0]! += vertex[0]!;
      position[1]! += vertex[1]!;
      position[2]! += vertex[2]!;
    }
    position[0]! /= objData.vertices.length;
    position[1]! /= objData.vertices.length;
    position[2]! /= objData.vertices.length;

    position[0] = Math.round(position[0]! * 10000) / 10000;
    position[1] = Math.round(position[1]! * 10000) / 10000;
    position[2] = Math.round(position[2]! * 10000) / 10000;

    allMarkerData.push({
      name: objData.name,
      position,
    });

    headerDeclarations += `extern const float ${objData.name}_position[3];\n`;

    sourceDefinitions += "// " + file + "\n";
    sourceDefinitions += `const float ${objData.name}_position[3] = { ${position.join(", ")} };\n\n`;

    console.log(
      `Processed marker: ${objData.name} at position (${position.join(", ")})`,
    );

    continue; // Skip object export
  }

  let currentMaterial: string | null = null;
  // Get texture data if material is used
  if (objData.material) {
    const mtlFile = file.replace(".obj", ".mtl");
    if (existsSync(mtlFile)) {
      const mtlContent = readFileSync(mtlFile, "utf-8");
      const mtlLines = mtlContent.split("\n");

      // Look for map_Kd
      for (const mtlLine of mtlLines) {
        const [mtlStart, ...mtlRest] = mtlLine.trim().split(/\s+/);
        if (!mtlStart) continue;
        if (mtlRest.length === 0) continue;

        if (mtlStart === "map_Kd") {
          currentMaterial = mtlRest.join(" ")!;
        }
      }

      let alreadyAdded = false;
      for (const textureData of allTextureData) {
        if (textureData.path === currentMaterial) {
          alreadyAdded = true;
          break;
        }
      }

      if (currentMaterial) {
        objData.material = currentMaterial;
      }

      // currentMaterial is now a path to the texture
      if (currentMaterial && !alreadyAdded) {
        // copy texture to data/
        copyFileSync(currentMaterial, `data/${basename(currentMaterial)}`);

        const textureData: TextureData = {
          path: currentMaterial,
          variableName: formatTextureVariable(currentMaterial),
          includePath: formatTextureInclude(currentMaterial),
        };

        allTextureData.push(textureData);
      }
    }
  }

  // Calculate normals
  for (let i = 0; i < finalVertices.length; i += 3) {
    const v0 = finalVertices[i]!;
    const v1 = finalVertices[i + 1]!;
    const v2 = finalVertices[i + 2]!;

    const edge1 = [v1[0]! - v0[0]!, v1[1]! - v0[1]!, v1[2]! - v0[2]!];
    const edge2 = [v2[0]! - v0[0]!, v2[1]! - v0[1]!, v2[2]! - v0[2]!];

    const normal = [
      edge1[1]! * edge2[2]! - edge1[2]! * edge2[1]!,
      edge1[2]! * edge2[0]! - edge1[0]! * edge2[2]!,
      edge1[0]! * edge2[1]! - edge1[1]! * edge2[0]!,
    ];

    const length = Math.sqrt(
      normal[0]! ** 2 + normal[1]! ** 2 + normal[2]! ** 2,
    );
    if (length > 0) {
      normal[0]! /= length;
      normal[1]! /= length;
      normal[2]! /= length;
    }

    objData.normals.push(normal);

    finalVertices[i]!.push(...normal);
    finalVertices[i + 1]!.push(...normal);
    finalVertices[i + 2]!.push(...normal);
  }

  allObjData.push(objData);

  // Write file

  headerDeclarations += `extern const int ${objData.name}_vertex_count;\n`;
  headerDeclarations += `extern const float ${objData.name}_vertices[${finalVertices.length}][8];\n`;

  sourceDefinitions += "// " + file + "\n";
  sourceDefinitions += `const int ${objData.name}_vertex_count = ${finalVertices.length};\n\n`;

  sourceDefinitions += `const float ${objData.name}_vertices[${finalVertices.length}][8] = {\n`;
  for (const vertex of finalVertices) {
    sourceDefinitions += `  { ${vertex.join(", ")} },\n`;
  }
  sourceDefinitions += "};\n";

  if (currentMaterial) {
    const textureData = allTextureData.find(
      (tex) => tex.path === currentMaterial,
    );
    if (textureData) {
      const index = allTextureData.indexOf(textureData!);
      if (textureData) {
        sourceDefinitions += `// Texture: ${textureData.path}\n`;
      }
      headerDeclarations += `extern const int ${objData.name}_texture;\n`;
      sourceDefinitions +=
        "const int " + objData.name + "_texture = " + index + ";\n";
    }
  }

  console.log("Processed object: " + objData.name);
}

// List textures
let textureIncludes = "";
for (const textureData of allTextureData) {
  textureIncludes += `#include "${textureData.includePath}"\n`;
}

headerDeclarations += "extern const int exported_texture_count;\n";
headerDeclarations += "extern const uint8_t* exported_textures[];\n";

sourceDefinitions +=
  "const int exported_texture_count = " + allTextureData.length + ";\n";
sourceDefinitions += "const uint8_t* exported_textures[] = {\n";
for (const textureData of allTextureData) {
  sourceDefinitions += ` (const uint8_t*)${textureData.variableName},\n`;
}
sourceDefinitions += "};\n\n";

sourceDefinitions += "\n";

// List texture indices for each object
headerDeclarations += "extern const int exported_objects_texture_indices[];\n";

sourceDefinitions += "const int exported_objects_texture_indices[] = {\n";
for (const objData of allObjData) {
  if (objData.material) {
    const textureData = allTextureData.find(
      (tex) => tex.path === objData.material,
    );
    if (textureData) {
      const index = allTextureData.indexOf(textureData!);
      sourceDefinitions += `  ${index},\n`;
    } else {
      sourceDefinitions += "  0,\n";
    }
  } else {
    sourceDefinitions += "  0,\n";
  }
}
sourceDefinitions += "};\n\n";

// List of all object vertices + counts
headerDeclarations += "extern const int exported_objects_vertex_counts[];\n";

sourceDefinitions += "const int exported_objects_vertex_counts[] = {\n";
for (const objData of allObjData) {
  sourceDefinitions += `  ${objData.name}_vertex_count,\n`;
}
sourceDefinitions += "};\n\n";

headerDeclarations += "extern const float* exported_objects_vertices[];\n";

sourceDefinitions += "const float* exported_objects_vertices[] = {\n";
for (const objData of allObjData) {
  sourceDefinitions += `  (const float*)${objData.name}_vertices,\n`;
}
sourceDefinitions += "};\n";

headerDeclarations += "extern const int exported_num_objects;\n";

sourceDefinitions += "\n";
sourceDefinitions +=
  "const int exported_num_objects = " + allObjData.length + ";\n";

const headerContent = `#pragma once
// Automatically generated by scripts/build_objs.ts

#include <stdint.h>

${headerDeclarations}`;

const sourceContent = `// Automatically generated by scripts/build_objs.ts

#include "exported.h"
${textureIncludes}

${sourceDefinitions}`;

writeFileSync(outHeaderPath, headerContent);
writeFileSync(outSourcePath, sourceContent);
