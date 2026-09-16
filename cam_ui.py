import os
import json
import google.generativeai as genai
import PIL.Image

genai.configure(api_key=os.environ.get("GEMINI_API_KEY"))

def process_optical_log(image_path):
    print(f"[!] HARDWARE TRIGGER: Optical Log Captured -> {image_path}")
    print("[*] SYNCING: Transmitting to Vision Engine...")
    
    try:
        img = PIL.Image.open(image_path)
    except FileNotFoundError:
        print("[ERROR] No image found. Save a photo as 'test_capture.jpg' first.")
        return
    
    model = genai.GenerativeModel('gemini-1.5-flash')
    
    prompt = """
    You are a backend processing engine for a screenless hardware device.
    Analyze this image. 
    - If it is food, return a JSON object with 'type': 'food', 'calories': number, 'protein_g': number, 'carbs_g': number, 'fat_g': number.
    - If it is a whiteboard or document, return a JSON object with 'type': 'document', 'transcription': 'text here', 'summary': 'short summary'.
    Return ONLY raw JSON. Do not include markdown formatting like ```json.
    """
    
    response = model.generate_content([prompt, img])
    
    print("\n[>] SYNC COMPLETE. Database Entry Generated:")
    
    # Validating the output is clean JSON
    try:
        parsed_data = json.loads(response.text.strip())
        print(json.dumps(parsed_data, indent=4))
    except json.JSONDecodeError:
        print("Raw Output (JSON parsing failed):")
        print(response.text)

if __name__ == "__main__":
    process_optical_log("test_capture.jpg")