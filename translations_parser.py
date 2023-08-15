import json
import pathlib


# Get a list of all the lines from a file
def get_all_lines_from_file(file_path):
    lines = []
    with open(file_path, 'r') as file:
        for line in file:
            lines.append(line)
    return lines


# Get a single string of all the lines from a file
def get_flat_lines_from_file(file_path):
    lines = get_all_lines_from_file(file_path)
    all_lines_flat = "".join(lines)
    return all_lines_flat


# Determine if the next character in a string is the same as the param ignoring whitespace characters (space, newline)
def next_character_is_param_ignore_whitespace(all_lines_flat, index, param):
    while all_lines_flat[index] == ' ' or all_lines_flat[index] == '\n':
        index += 1

    return all_lines_flat[index] == param


# Get of the occurrences of a function from a file (returns a list of strings corresponding to the parameters passed
# to the function)
def get_all_occurrences_of_function_from_file(file_path, function_name):
    all_lines_flat = get_flat_lines_from_file(file_path)

    # Find all the occurrences of the function name in the file
    occurrences = []
    i = 0
    while (i := all_lines_flat.find(function_name, i + len(function_name))) != -1:
        occurrences.append(i)

    strings_to_translate = []
    begin_bracket = -1
    begin_quote = -1
    for occurrence in occurrences:
        # Find the beginning of the string passed to the function "function_name"
        if next_character_is_param_ignore_whitespace(all_lines_flat, occurrence + len(function_name), '('):
            begin_bracket = occurrence + all_lines_flat[occurrence:].find("(")
        if next_character_is_param_ignore_whitespace(all_lines_flat, begin_bracket + 1, '\"'):
            begin_quote = begin_bracket + all_lines_flat[begin_bracket:].find('\"')

        if begin_quote == -1 or begin_bracket == -1 or begin_quote <= begin_bracket:
            continue

        # Find the end of the string (ignores escaped quotes)
        end_quote = begin_quote + 1
        diff = 0

        while (diff := diff + 1 + all_lines_flat[end_quote + diff:].find('\"')) != -1:
            if all_lines_flat[end_quote + diff - 2] != '\\':
                break

        end_quote += diff - 2

        strings_to_translate.append(all_lines_flat[begin_quote + 1: end_quote + 1])

    return strings_to_translate


# Get all the strings to translate from all the files in the current directory (in cpp, h, hpp files)
def parse_files():
    # Get the current path
    base_path = pathlib.Path(__file__).parent.absolute() / "src"
    print('Parsing files in directory: ' + str(base_path) + '\n')

    # Get the paths of all the files in that directory (recursively)
    file_paths = [path for path in base_path.glob('**/*')]
    code_files_paths = [path for path in file_paths if
                        path.suffix == '.cpp' or path.suffix == '.h' or path.suffix == '.hpp']

    # Get all the strings to translate from all the files with suffix .cpp, .h, .hpp
    strings_to_translate = []
    for path in code_files_paths:
        strings_to_translate = get_all_occurrences_of_function_from_file(path, "TR")
        strings_to_translate.extend(get_all_occurrences_of_function_from_file(path, "TRC"))

    return list(set(strings_to_translate))


# Write all the strings that need to be translated to a file
def write_untranslated_strings_to_file(strings_to_translate, language_code):
    translated_strings = []
    language_file_path = f"translations/{language_code}.json"

    # Check if the language file exists
    if not pathlib.Path(language_file_path).exists():
        print(f"Language file {language_file_path} does not exist")
        return

    # Read the language file and get all the strings that are already translated
    with open(language_file_path, 'r') as file:
        print(f"Reading {language_file_path}")
        translations_json = json.loads(file.read())
        translated_strings = [translated_string for translated_string in translations_json.keys()]
        print(f"Found {len(translated_strings)} translated strings in language file: {translated_strings}\n")

    # Write all the strings that are not already translated to a file
    with open(f"translations/{language_code}_untranslated.txt", 'w') as file:
        print(f"Writing to {language_file_path}_untranslated.txt")

        untranslated_strings = [untranslated_string for untranslated_string in strings_to_translate
                                if untranslated_string not in translated_strings]
        print(f"Found {len(untranslated_strings)} untranslated strings in code: {untranslated_strings}")

        for string_to_translate in untranslated_strings:
            file.write(string_to_translate + '\n')


# Run the script
def run():
    strings_to_translate = parse_files()
    print(f"Found {len(strings_to_translate)} strings that should be translated: {strings_to_translate}\n")

    language_code_to_translate_to = input("Enter the language code to translate to (e.g. fr) (write all to check all "
                                          "existing languages): ")

    if language_code_to_translate_to == "all":
        for language_code in pathlib.Path("build-release/_deps/trandslateor-src/translations").glob("*.json"):
            if language_code.stem == "en" or language_code.stem == "languages":
                continue
            language_code_to_translate_to = language_code.stem
            print("Checking translations for language code: " + language_code_to_translate_to + "\n")
            write_untranslated_strings_to_file(strings_to_translate, language_code_to_translate_to)
    else:
        print("Checking translations for language code: " + language_code_to_translate_to + "\n")
        write_untranslated_strings_to_file(strings_to_translate, language_code_to_translate_to)


if __name__ == '__main__':
    run()
