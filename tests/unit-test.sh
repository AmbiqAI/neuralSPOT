#!/bin/bash

# Define the directory path
directory="../neuralspot"

# Loop through each library in the neuralspot directory
for library in "$directory"/*; do
    tests_directory="$library/tests"
    echo "testing $tests_directory"
    # Check if the file is a library
    if [[ -d "$tests_directory" ]]; then
        ini_file=$(find "$tests_directory" -maxdepth 1 -type f -name "*.ini")
        #Determine which Python command to use
        if python3 --version &>/dev/null; then
            python_cmd="python3"
        elif python --version &>/dev/null; then
            python_cmd="python"
        else
            echo "Error: Python interpreter not found."
            exit 1
        fi
        if [[ -f "$ini_file" ]]; then
            # Save current directory
            pushd . > /dev/null
            # Change to directory containing ns_test
            cd "../tools"
            "$python_cmd" -m ns_test --test-directory "$tests_directory"
            # Restore original directory
            popd > /dev/null

        fi
    fi
done


# Check test results
failed_tests=()
directory="../projects/tests"
for test in "$directory"/*/; do
    txt_file=$(find "$test" -maxdepth 1 -type f -name "*.txt")
    if [[ -f "$txt_file" ]]; then
        if grep -q "FAIL" "$txt_file"; then
            failed_tests+=("$test")
        fi
    else
        echo "No test results found."
        exit 1
    fi
done

# Print test results
if [[ ${#failed_tests[@]} -gt 0 ]]; then
    echo "Failed tests:"
    for failed_test in "${failed_tests[@]}"; do
        echo "$failed_test"
    done
else
    echo "All tests passed."
fi

