import serial.tools.list_ports
import os
import sys
import subprocess
import time

def list_serial_ports():
    ports = serial.tools.list_ports.comports()
    return ports

def get_current_directory():
    return os.getcwd()

def run_docker_command(is_flash=False):
    current_directory = get_current_directory()

    # Base Docker run command
#    docker_command = [
#        "docker", "run", "--rm", "-v", f"{current_directory}:/project", "-w", "/project", 
#        "-e", "HOME=/tmp", "-it", "--name", "UCF-Senior-Design", "espressif/idf"
#    ]

    docker_command = [
       "docker", "run", "--rm", "--build", "-v", f"{current_directory}:/project", "-w", "/project", 
        "-e", "HOME=/tmp", "-it", "--name", "UCF-Senior-Design", "--file", "./Dockerfile", "espressif/idf"
    ]
    
    if is_flash:
        docker_command.append("idf.py")
        docker_command.extend(["--port", "rfc2217://host.docker.internal:4000?ign_set_control", "flash"])

    # Print the docker command that will be executed
    print(f"\nRunning Docker command:\n{' '.join(docker_command)}\n")

    # Wait for 1 second before executing the command
    time.sleep(1)

    # Run Docker command in the same terminal
    subprocess.run(docker_command)

def run_rfc2217_server(serial_port):
    print(f"\nStarting RFC2217 server on {serial_port}...\n")
    rfc_command = ["python", "esp_rfc2217_server.py", "-v", "-p", "4000", serial_port]
    
    # Run RFC2217 server in the background and capture output
    try:
        rfc_process = subprocess.Popen(rfc_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        time.sleep(1)  # Allow time for the server to start

        # Check if the server started successfully
        if rfc_process.poll() is None:
            print("RFC2217 server started successfully.\n")
        else:
            # Capture and display error output
            stdout, stderr = rfc_process.communicate()
            print("Failed to start RFC2217 server. Error output:\n")
            print(stderr.decode())
            sys.exit(1)  # Exit if the server couldn't start
    except Exception as e:
        print(f"Error starting RFC2217 server: {e}\n")
        sys.exit(1)

def main():
    ports = list_serial_ports()
    if not ports:
        print("No serial ports found.")
        continue_choice = input(
            "No matches found. Would you like to continue into the Docker container (you won't be able to interact with serial ports)? (y/n): ").strip().lower()
        
        if continue_choice == 'y':
            run_docker_command()
        else:
            print("Please connect the device and press any key to try again.")
            input()  # Wait for user input
            main()  # Retry finding the serial ports
    
    else:
        print("Available serial ports:")
        for port in ports:
            print(f" - {port.device}: {port.description}")
        
        match_found = False
        for port in ports:
            if "Silicon Labs CP210x USB to UART Bridge" in port.description:
                print(f"Match found: {port.device} -> {port.description}")
                match_found = True
                # Run RFC2217 server before entering Docker container
                run_rfc2217_server(port.device)
                break
        
        if not match_found:
            continue_choice = input(
                "No matching serial port found. Would you like to continue into the Docker container (you won't be able to interact with serial ports)? (y/n): ").strip().lower()
            
            if continue_choice == 'y':
                run_docker_command()
            else:
                print("Please connect the device and press any key to try again.")
                input()  # Wait for user input
                main()  # Retry finding the serial ports
        else:
            flash_choice = input("Would you like to flash the device? (y/n): ").strip().lower()
            if flash_choice == 'y':
                run_docker_command(is_flash=True)
            else:
                continue_choice = input(
                    "Would you like to continue into the Docker container? (y/n): ").strip().lower()
                if continue_choice == 'y':
                    # Run the command with `-it` flag for interactivity
                    current_directory = get_current_directory()
                    docker_command = [
                        "docker", "build", "-t", "Senior-Design-Image", ".", "&&", "docker", "run", "--rm", "-v", f"{current_directory}:/project", "-w", "/project", 
                        "-e", "HOME=/tmp", "-it", "--name", "UCF-Senior-Design", "espressif/idf"
                    ]
                    # Print the docker command that will be executed
                    print(f"\nRunning Docker command:\n{' '.join(docker_command)}\n")

                    # Wait for 1 second before executing the command
                    time.sleep(1)

                    # Run Docker command in the same terminal
                    subprocess.run(docker_command)
                else:
                    print("Exiting the program.")
                    sys.exit(0)

if __name__ == "__main__":
    main()

