import websocket
import json

def interactive_ws_api():
    ws = websocket.create_connection("ws://192.168.1.86:80/ws")

    try:
        while True:
            print("\nChoose an action:")
            print("1. Add Filament")
            print("2. Get Filament")
            print("3. Update Filament")
            print("4. Remove Filament")
            print("5. Exit")

            choice = input("Enter your choice: ")

            if choice == "1":
                motor_id = int(input("Enter motor_id: "))
                metadata = input("Enter metadata (JSON string): ")
                request = {
                    "type": "filament",
                    "action": "add",
                    "motor_id": motor_id,
                    "metadata": metadata
                }
            elif choice == "2":
                filament_id = int(input("Enter filament ID: "))
                request = {
                    "type": "filament",
                    "action": "get",
                    "id": filament_id
                }
            elif choice == "3":
                filament_id = int(input("Enter filament ID: "))
                motor_id = int(input("Enter new motor_id: "))
                metadata = input("Enter new metadata (JSON string): ")
                request = {
                    "type": "filament",
                    "action": "update",
                    "id": filament_id,
                    "motor_id": motor_id,
                    "metadata": metadata
                }
            elif choice == "4":
                filament_id = int(input("Enter filament ID: "))
                request = {
                    "type": "filament",
                    "action": "remove",
                    "id": filament_id
                }
            elif choice == "5":
                print("Exiting...")
                break
            else:
                print("Invalid choice. Please try again.")
                continue

            ws.send(json.dumps(request))
            response = ws.recv()
            print("Response:", response)

    except Exception as e:
        print("Error:", e)
    finally:
        ws.close()

if __name__ == "__main__":
    interactive_ws_api()