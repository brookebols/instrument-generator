import json
import random

def parse_input(input_str):
    criteria = {}
    lines = input_str.strip().split(';')
    for line in lines:
        if not line:
            continue
        key, value = line.split(':')
        criteria[key.strip()] = [v.strip() for v in value.split(',')]
    return criteria

def load_json_file(file_path):
    with open(file_path, 'r') as file:
        return json.load(file)

def match_criteria(entry, criteria):
    for key, values in criteria.items():
        if key == 'Pitch' or key == 'Velocity':
            continue  # Skip pitch and velocity here, handle them separately
        elif key.replace(' ', '_').lower() + '_str' in entry:
            if not any(value in entry[key.replace(' ', '_').lower() + '_str'] for value in values):
                return False
    return True

def find_matching_pitch_groups(data, pitch_range):
    pitch_groups = {}
    for key, entry in data.items():
        pitch = entry['pitch']
        if pitch_range[0] <= pitch <= pitch_range[1]:
            if pitch not in pitch_groups:
                pitch_groups[pitch] = []
            pitch_groups[pitch].append(entry)
    return pitch_groups

def find_entries(data, criteria):
    pitch_values = criteria.pop('Pitch')[0].split('-')
    pitch_range = (int(pitch_values[0]), int(pitch_values[1]))

    pitch_groups = find_matching_pitch_groups(data, pitch_range)
    for pitch, entries in pitch_groups.items():
        matching_entries = [entry for entry in entries if match_criteria(entry, criteria)]
        if len(matching_entries) >= 2:
            return random.sample(matching_entries, 2)  # Return two random matching entries

    return []  # No matching entries found

def select_audio(input_str, path_to_json):

    criteria = parse_input(input_str)
    data = load_json_file(path_to_json)
    matching_entries = find_entries(data, criteria)

    if matching_entries:
        print("Found 2 entries with matching pitches:")
        for entry in matching_entries:
            print(entry)
    else:
        print("No matching entries found with matching pitches.")

