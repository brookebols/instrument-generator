import os
import numpy as np
import matplotlib.pyplot as plt
from magenta.models.nsynth import utils
from magenta.models.nsynth.wavenet import fastgen
from IPython.display import Audio
import tensorflow as tf
import time
from concurrent.futures import ProcessPoolExecutor

checkpoint_path = 'model.ckpt-200000'

print("Num GPUs Available: ", len(tf.config.experimental.list_physical_devices('GPU')))
if tf.config.experimental.list_physical_devices('GPU'):
    print("GPU is detected.")
else:
    print("GPU not detected.")


def load_encoding(fname, sample_length=64000, sr=16000, ckpt=checkpoint_path):
    audio = utils.load_audio(fname, sample_length=sample_length, sr=sr)
    encoding = fastgen.encode(audio, ckpt, sample_length)
    return audio, encoding


def process_pitch_group(pitch, pitch_group, path_to_audio):
    # Initialize a variable to hold the mixed encoding, weighted by each instrument's weight
    mixed_encoding = None

    for instrument_info in pitch_group:
        instrument_file = f"{path_to_audio}{instrument_info['note_str']}.wav"
        weight = instrument_info['weight']

        # Load the instrument audio file
        audio, encoding = load_encoding(instrument_file)

        # Apply the weight to the encoding
        weighted_encoding = encoding * weight

        # Mix the weighted encoding with the mixed_encoding
        if mixed_encoding is None:
            mixed_encoding = weighted_encoding
        else:
            mixed_encoding += weighted_encoding

    # After all instrument encodings are mixed according to their weights, synthesize the mixed audio
    synthesized_audio = fastgen.synthesize(mixed_encoding, save_paths=[f'synthesized_audio2/{pitch}.wav'])

    # Save or return the synthesized audio
    return synthesized_audio

'''

def process_pitch_groups(pitch_groups, path_to_audio):
    # This dictionary will hold the mixed encodings for each pitch
    mixed_encodings = {}
    save_paths = []

    # Iterate over each pitch group in the batch
    for pitch, pitch_group in pitch_groups.items():
        mixed_encoding = None

        for instrument_info in pitch_group:
            instrument_file = f"{path_to_audio}{instrument_info['note_str']}.wav"
            weight = instrument_info['weight']

            # Load the instrument audio file
            audio, encoding = load_encoding(instrument_file)

            # Apply the weight to the encoding
            weighted_encoding = encoding * weight

            # Mix the weighted encoding with the mixed_encoding for the current pitch
            if mixed_encoding is None:
                mixed_encoding = weighted_encoding
            else:
                mixed_encoding += weighted_encoding

        mixed_encodings[pitch] = mixed_encoding
        save_paths.append(f'synthesized_audio/{pitch}.wav')

    # Convert mixed_encodings to a list of encodings in the same order as save_paths
    encodings_batch = [mixed_encodings[pitch] for pitch in pitch_groups.keys()]

    # Batch synthesize the audio for all pitches in the batch
    synthesized_audios = fastgen.synthesize(np.array(encodings_batch), save_paths=save_paths)

    return synthesized_audios'''


def main(input_dict, path_to_audio):
    start = time.time()

    for pitch, pitch_group in input_dict.items():
        start_pitch = time.time()
        process_pitch_group(pitch, pitch_group, path_to_audio)

        # Process all pitch groups in a single batch
        #process_pitch_groups(input_dict, path_to_audio)

        print(f"Pitch {pitch} synthesized in {time.time() - start_pitch} seconds.")
    
    print(f"{len(input_dict)} audio files synthesized in {time.time() - start} seconds.")
