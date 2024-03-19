import select_audio
import select_range_audio
import synthesize_clips
import json

input_str = """Instrument #1
Instrument Family: keyboard;
Audio Qualities: dark, long_release;
Instrument Source: acoustic;
Velocity: 50-50;
Weight: 0.7;

Instrument #2
Instrument Family: bass;
Audio Qualities: dark, reverb;
Instrument Source: acoustic;
Velocity: 50-50;
Weight: 0.3;"""


path_to_json = 'C:/NSynth/nsynth-train/examples.json'
path_to_audio = 'C:/NSynth/nsynth-train/audio/'


def main():
    selections = select_range_audio.select_audio(input_str, path_to_json)
    #formatted_selections = json.dumps(selections, indent=4, sort_keys=True)
    #print(formatted_selections)
    results = synthesize_clips.main(selections, path_to_audio)



if __name__ == "__main__":
    main()