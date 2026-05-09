
# Find all .mp3s in sd/beatmaps/** and convert them to .ogg, as "audio.mp3.ogg" in the same directory
find ./sd/beatmaps -type f -name "*.mp3" -print0 | while IFS= read -r -d '' mp3; do
    ogg="${mp3}.ogg"
    if [ ! -f "$ogg" ]; then
        echo "Converting $mp3 to $ogg"
        ffmpeg -nostdin -i "$mp3" -c:a libvorbis -q:a 5 "$ogg"
    else
        echo "Ogg already exists for $mp3, skipping"
    fi
done
done