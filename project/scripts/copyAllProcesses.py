# this file should not be ran manually
import os
import shutil
from typing import Dict, List
PROCESS_BINARIES_FOLDER : str = "bin/processes/"

def get_files_in_folders(folder_path: str) -> Dict[str, List[str]]:
    process_files: Dict[str, List[str]] = {}

    for process_folder in os.listdir(folder_path):
        process_folder_path = os.path.join(folder_path, process_folder)
        if os.path.isdir(process_folder_path):
            files: List[str] = []
            for root, _, filenames in os.walk(process_folder_path):
                for filename in filenames:
                    relative_path = os.path.relpath(os.path.join(root, filename), folder_path)
                    files.append(relative_path)
            process_files[process_folder] = files

    return process_files

def get_file_content(file: str) -> str:
    with open(file, 'r') as file:
        content = file.read()
    return content

def create_file_duplicate(file: str, file_content: str):
    with open(file, 'w') as destination_file:
        destination_file.write(file_content)

def upload_declaration_of_main_location(file_content: str) -> str:
    """test if the file contains a main function and puts an attribute that will make sure that this function will be at the address zero

    Args:
        file_content (str): _description_ the content of the file that is checked if there is a main function in it
    """
    lines = file_content.splitlines()
    for i, line in enumerate(lines):
        if "int main(" in line:
            lines.insert(i, 'int main() __attribute__((section(".main_location")));')
            break
    return '\n'.join(lines)

def copy_all_file(process_dict: Dict[str, List[str]], old_relative_path: str, new_relative_path: str):
    file_content: str = ""
    directory: str = ""
    new_file_full_file_path: str = ""
    old_file_full_file_path: str = ""

    if os.path.exists(new_relative_path):
        shutil.rmtree(new_relative_path)

    for key, value in process_dict.items():
        for file in value:
            old_file_full_file_path = old_relative_path + file
            file_content = upload_declaration_of_main_location(get_file_content(old_file_full_file_path))

            new_file_full_file_path = new_relative_path + file
            directory = os.path.dirname(new_file_full_file_path)
            if not os.path.exists(directory):
                os.makedirs(directory)
            
            create_file_duplicate(new_file_full_file_path, file_content)
            
def remove_processes_binaries():
    if os.path.exists(PROCESS_BINARIES_FOLDER):
        shutil.rmtree(PROCESS_BINARIES_FOLDER)

processes_src_path: str = 'src/processes/'
processes_dst_path: str = 'processes/code/'


process_files_dict: Dict[str, List[str]] = get_files_in_folders(processes_src_path)
copy_all_file(process_files_dict, processes_src_path, processes_dst_path)
remove_processes_binaries()