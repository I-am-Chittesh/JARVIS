import os
import json
import time
import tkinter as tk
from PIL import Image, ImageTk

def run_optical_ui(image_name):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    image_path = os.path.join(script_dir, image_name)
    processed_img_path = os.path.join(script_dir, "optimized_capture.png")
    db_path = os.path.join(script_dir, "local_database.json")

    if not os.path.exists(image_path):
        print(f"[ERROR] Cannot find {image_path}")
        return

    # 1. Process the Image
    with Image.open(image_path) as img:
        width, height = img.size
        original_kb = round(os.path.getsize(image_path) / 1024, 2)
        
        # Create a display copy for the UI
        display_img = img.copy()
        display_img.thumbnail((350, 350))
        
        # Create the optimized edge-node copy
        optimized_img = img.convert("L")
        optimized_img.thumbnail((256, 256))
        optimized_img.save(processed_img_path, format="PNG", optimize=True)
        new_kb = round(os.path.getsize(processed_img_path) / 1024, 2)

    # 2. Build the Payload and Save to DB
    payload = {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        "device": "jarvis_edge_node_01",
        "action": "optical_shutter",
        "telemetry": {
            "resolution": f"{width}x{height}",
            "original_kb": original_kb,
            "optimized_kb": new_kb,
            "bandwidth_saved": round(original_kb - new_kb, 2)
        },
        "status": "LOCAL_SYNC_SUCCESS"
    }

    with open(db_path, "a") as db_file:
        db_file.write(json.dumps(payload) + "\n")

    # 3. Launch the Graphical Window
    root = tk.Tk()
    root.title("J.A.R.V.I.S. - Edge Node Optical UI")
    root.geometry("750x400")
    root.configure(bg="#0d0d0d")

    # Left Side: Image Display
    left_frame = tk.Frame(root, bg="#0d0d0d", width=400)
    left_frame.pack(side="left", fill="y", padx=20, pady=20)
    
    tk_img = ImageTk.PhotoImage(display_img)
    img_label = tk.Label(left_frame, image=tk_img, bg="#0d0d0d", bd=2, relief="solid")
    img_label.image = tk_img
    img_label.pack()

    # Right Side: Data Telemetry
    right_frame = tk.Frame(root, bg="#0d0d0d")
    right_frame.pack(side="right", fill="both", expand=True, padx=20, pady=20)

    title_label = tk.Label(right_frame, text="OPTICAL SYNC COMPLETE", fg="#00ff00", bg="#0d0d0d", font=("Courier", 16, "bold"))
    title_label.pack(anchor="w", pady=(0, 10))

    # Text box for the JSON payload
    data_text = tk.Text(right_frame, height=12, width=40, bg="#1a1a1a", fg="#00ffff", font=("Courier", 10), relief="flat")
    data_text.pack(anchor="w")
    data_text.insert(tk.END, json.dumps(payload, indent=4))
    data_text.config(state="disabled") # Make it read-only
    
    footer_label = tk.Label(right_frame, text=f"Data appended to local_database.json", fg="#555555", bg="#0d0d0d", font=("Courier", 9))
    footer_label.pack(anchor="w", pady=(10, 0))

    # Keep the window open until you close it
    root.mainloop()

if __name__ == "__main__":
    run_optical_ui("test_capture.png")