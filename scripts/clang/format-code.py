import argparse
import os
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET

allowed_extensions = [".cpp", ".hpp", ".c", ".h"]

ignore_files = ["json/json.hpp", "sha256", "tinyxml"]


class Formatter:

    Executable = "clang-format"

    def __init__(self):
        if shutil.which(Formatter.Executable) is None:
            raise Exception("clang-format is not exists")

    def collect_files(self, paths):
        files = []

        def add_file(file_path):
            need_ignore = False

            for i in ignore_files:
                if i.lower() in file_path:
                    need_ignore = True
                    break

            if not need_ignore:
                files.append(file_path)

        for d in paths:
            if not os.path.exists(d):
                continue
            if os.path.isdir(d):
                for root, _, filenames in os.walk(d):
                    for filename in filenames:
                        _, ext = os.path.splitext(filename)
                        if ext in allowed_extensions:
                            add_file(os.path.join(root, filename))
            elif os.path.splitext(d)[1] in allowed_extensions:
                add_file(d)
        return files

    def format(self, search_paths):
        files = self.collect_files(search_paths)

        for f in files:
            print(f"Format file {f}")
            cmd = f"{Formatter.Executable} -i {f}"
            with subprocess.Popen(cmd, shell=True, cwd=os.getcwd(), stdout=None, stderr=None) as process:
                rc = process.wait()
                if rc != 0:
                    raise f"Error while formatting {f}"
            
            cmd_check_newline = f"tail -c 1 < '{f}' | read -r _ || echo >> '{f}' "

            with subprocess.Popen(
                    cmd_check_newline, shell=True, cwd=os.getcwd(),
                    stdout=subprocess.PIPE, stderr=subprocess.PIPE
                    ) as process_check_new_line:
                rc_newline = process_check_new_line.wait()
                if rc_newline != 0:
                    raise f"Error adding newline in {f}"


    def check(self, search_paths) -> bool:
        files = self.collect_files(search_paths)

        final_result = True
        non_formatted_files = []

        for f in files:
            print(f"Check {f}", end="")
            cmd = f"{Formatter.Executable} --output-replacements-xml {f}"
            cmd_check_newline = f"tail -n 1 < '{f}' | read -r _"

            with subprocess.Popen(
                    cmd_check_newline, shell=True, cwd=os.getcwd(),
                    stdout=subprocess.PIPE, stderr=subprocess.PIPE
                    ) as process_check_new_line:
                rc_newline = process_check_new_line.wait()

            with subprocess.Popen(
                cmd, shell=True, cwd=os.getcwd(), stdout=subprocess.PIPE, stderr=subprocess.PIPE
            ) as process:
                rc = process.wait()
                output = process.communicate()[0]

            passed = False

            if rc == 0 and rc_newline == 0:

                passed = True
                # check if there are any replacements
                s = output.decode("utf-8")
                if len(s) > 0:
                    root = ET.fromstring(s)
                    replacements = root.findall("replacement")

                    if len(replacements) > 0:
                        passed = False

            if not passed:
                final_result = False
                non_formatted_files.append(f)

            print(f" - {'ok' if passed else 'fail' }")

        return (final_result, non_formatted_files)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Process some integers.")
    parser.add_argument(
        "files",
        nargs="*",
        help="List of input files or folders (relative to the working directory)",
    )
    parser.add_argument("--check", action="store_true")

    args = parser.parse_args()

    c = Formatter()
    if args.check:
        is_passed, output_files = c.check(args.files)
        if not is_passed:
            if len(output_files) > 0:
                print("-" * 10)
                print("Files that need formatting:")
                for file in output_files:
                    print(f" {file}")
        else:
            print("-" * 10)
            print("All checks passed")

        sys.exit(0 if is_passed else 1)
    else:
        c.format(args.files)
