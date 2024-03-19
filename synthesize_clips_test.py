import os
import time

import numpy as np
import matplotlib.pyplot as plt
from magenta.models.nsynth import utils
from magenta.models.nsynth.wavenet import fastgen
from IPython.display import Audio
import time

checkpoint_path = 'wavenet-ckpt/wavenet-ckpt/model.ckpt-200000'


def load_encoding(fname, sample_length=None, sr=16000, ckpt=checkpoint_path):
    audio = utils.load_audio(fname, sample_length=sample_length, sr=sr)
    encoding = fastgen.encode(audio, ckpt, sample_length)
    return audio, encoding


start = time.time()

sample_length = 16000

aud1, enc1 = load_encoding('flute_acoustic_004-069-075.wav', sample_length)
aud2, enc2 = load_encoding('brass_acoustic_022-069-025.wav', sample_length)

enc_mix = (enc1 + enc2) / 2.0

try:
    fastgen.synthesize(enc_mix, save_paths=['mix_test2.wav'])
    print(f"Time taken: {time.time() - start} seconds.")
except Exception as e:
    print(e)
