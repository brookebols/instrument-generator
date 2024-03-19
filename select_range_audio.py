import json

def parse_input(input_str):
    instruments = input_str.strip().split('\n\n')  # Split input by double newlines to separate instruments
    parsed_instruments = []
    weights = []

    for instrument in instruments:
        criteria = {}
        lines = instrument.strip().split('\n')  # Split each instrument block by newline
        for line in lines:
            if not line or line.startswith('Instrument #'):
                continue  # Skip empty lines and instrument headers
            key, value = line.split(':')
            key = key.strip()
            value = value.strip().rstrip(';')  # Remove any trailing semicolon

            if key == 'Weight':
                weights.append(float(value))  # Store weight separately
            else:
                criteria[key] = [v.strip() for v in value.split(',')]  # Split values by comma for lists

        parsed_instruments.append(criteria)

    return parsed_instruments, weights


def load_json_file(file_path):
    with open(file_path, 'r') as file:
        return json.load(file)

def match_criteria(entry, criteria):
    match_score = 0  # Initialize match score

    # Check for instrument family match first
    if 'Instrument Family' in criteria and entry['instrument_family_str'] in criteria['Instrument Family']:
        match_score += 1  # Add a point for instrument family match
    else:
        return 0  # Immediate rejection if the instrument family doesn't match

    for key, values in criteria.items():
        if key == 'Instrument Family':
            continue  # Already checked instrument family, so skip it

        entry_key = key.replace(' ', '_').lower() + '_str'  # Format the entry key to match JSON structure
        if entry_key in entry:
            entry_value = entry[entry_key]
            if isinstance(entry_value, list):
                # Add a point for each matching value in the list
                match_score += sum(1 for value in values if value in entry_value)
            elif entry_value in values:
                match_score += 1  # Add a point for a direct match

    return match_score



'''
def match_criteria(entry, criteria):
    match_count = 0
    if 'Instrument Family' in criteria:
        family_criteria = criteria['Instrument Family']
        entry_family = entry['instrument_family_str']
        if any(family == entry_family for family in family_criteria):
            match_count += 1
        else:
            return 0  # If the instrument family doesn't match, return 0

    for key, values in criteria.items():
        if key in ['Pitch', 'Velocity', 'Instrument Family']:
            continue
        elif key.replace(' ', '_').lower() + '_str' in entry:
            entry_value = entry[key.replace(' ', '_').lower() + '_str']
            if isinstance(entry_value, list):
                if any(value in entry_value for value in values):
                    match_count += 1
            elif any(value == entry_value for value in values):
                match_count += 1

    return match_count
'''


'''def match_criteria(entry, criteria):
    # First, check for an exact match on the instrument family
    if 'Instrument Family' in criteria:
        family_criteria = criteria['Instrument Family']
        entry_family = entry['instrument_family_str']
        if not any(family == entry_family for family in family_criteria):
            return False  # If the instrument family doesn't match, reject the entry immediately

    # Then, check other criteria
    for key, values in criteria.items():
        if key in ['Pitch', 'Velocity', 'Instrument Family']:
            continue  # Skip pitch, velocity, and instrument family here, as they are handled separately
        elif key.replace(' ', '_').lower() + '_str' in entry:
            entry_value = entry[key.replace(' ', '_').lower() + '_str']
            if isinstance(entry_value, list):
                if not any(value in entry_value for value in values):
                    return False
            elif not any(value == entry_value for value in values):
                return False

    return True'''


def find_overlapping_pitch_range(data, parsed_instruments):
    # Initialize with the widest possible pitch range
    overlapping_range = (None, None)

    for instrument in parsed_instruments:
        for key, entry in data.items():
            pitch = entry['pitch']
            if entry['instrument_family_str'] in instrument['Instrument Family']:
                if overlapping_range[0] is None or pitch < overlapping_range[0]:
                    overlapping_range = (pitch, overlapping_range[1])
                if overlapping_range[1] is None or pitch > overlapping_range[1]:
                    overlapping_range = (overlapping_range[0], pitch)

    return overlapping_range


def find_matching_pitch_groups(data, pitch_range):
    pitch_groups = {}
    for key, entry in data.items():
        pitch = entry['pitch']
        if pitch_range[0] <= pitch <= pitch_range[1]:
            if pitch not in pitch_groups:
                pitch_groups[pitch] = []
            pitch_groups[pitch].append(entry)
    return pitch_groups


def find_entries_for_instrument(data, instrument_criteria, pitch_groups):
    consistent_entries = {}

    for pitch, entries in pitch_groups.items():
        highest_match_score = -1  # Initialize with a value lower than the lowest possible score
        best_match_entry = None

        for entry in entries:
            match_score = match_criteria(entry, instrument_criteria)

            # Update the best match if the current entry has a higher match score
            if match_score > highest_match_score:
                highest_match_score = match_score
                best_match_entry = entry

        # After evaluating all entries for this pitch, select the one with the highest match score
        if best_match_entry is not None:
            consistent_entries[pitch] = best_match_entry

    return consistent_entries



'''
def find_entries_for_instrument(data, instrument_criteria, pitch_groups):
    matching_entries = {}
    best_match_entries = {}
    for pitch, entries in pitch_groups.items():
        best_match_count = 0
        best_match = None
        for entry in entries:
            match_count = match_criteria(entry, instrument_criteria)
            if match_count == len(instrument_criteria):
                matching_entries[pitch] = entry
                break
            elif match_count > best_match_count:
                best_match_count = match_count
                best_match = entry
        if pitch not in matching_entries and best_match is not None:
            best_match_entries[pitch] = best_match
    return matching_entries if matching_entries else best_match_entries

'''

def find_entries(data, parsed_instruments, weights):
    # Calculate the overlapping pitch range
    #overlapping_pitch_range = find_overlapping_pitch_range(data, parsed_instruments)
    overlapping_pitch_range = (48, 60)  # for testing

    # Find pitch groups within the overlapping pitch range
    pitch_groups = find_matching_pitch_groups(data, overlapping_pitch_range)

    # Find matching entries for each instrument and include the weight
    instrument_entries = []
    for criteria, weight in zip(parsed_instruments, weights):
        entries_with_weight = find_entries_for_instrument(data, criteria, pitch_groups)
        # Append weight to each entry
        for pitch, entry in entries_with_weight.items():
            entry['weight'] = weight  # Add weight to the entry
        instrument_entries.append(entries_with_weight)

    # Initialize a dictionary to hold the final selected entries grouped by pitch
    final_selection = {}

    # Iterate over pitches in the overlapping range
    for pitch in pitch_groups.keys():
        if all(pitch in entries for entries in instrument_entries):
            # If both instruments have an entry for this pitch, add them to the final selection
            final_selection[pitch] = [entries[pitch] for entries in instrument_entries]

    return final_selection


def select_audio(input_str, path_to_json):
    parsed_instruments, weights = parse_input(input_str)
    data = load_json_file(path_to_json)
    final_selection = find_entries(data, parsed_instruments, weights)

    if final_selection:
        print("Selected entries grouped by pitch (sorted), including weights:")
        # Sort the pitches (keys of the dictionary) before iterating
        for pitch in sorted(final_selection.keys()):
            entries = final_selection[pitch]
            print(f"Pitch: {pitch}")
            for entry in entries:
                print(f"Entry: {entry}, Weight: {entry['weight']}")

        return final_selection
    else:
        print("No matching entries found.")






