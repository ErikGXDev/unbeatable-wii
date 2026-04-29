// Delete objects in assets/exported and source/generated
import { rm } from "fs/promises";

await rm("assets/exported", { recursive: true, force: true });
await rm("source/generated", { recursive: true, force: true });
