import tkinter as tk
from tkinter import ttk, messagebox
import random, time, json, os, threading

# ---------------- ENUMS ----------------
LANDING, TAKEOFF = "Landing", "Takeoff"
NORMAL, EMERGENCY = "Normal", "Emergency"
PENDING, PROCESSED, CANCELLED = "Pending", "Processed", "Cancelled"

EMERGENCY_TYPES = ["None", "Engine Failure", "Medical", "Fuel Low"]
EMERGENCY_PRIORITY = {"Engine Failure": 3, "Medical": 2, "Fuel Low": 1, "None": 0}

# ---------------- FLIGHT CLASS ----------------
class Flight:
    def __init__(self, flight_id, name, ftype, priority, emergency_type):  # FIXED: __init__
        self.id = flight_id
        self.name = name
        self.type = ftype
        self.priority = priority
        self.emergency = emergency_type
        self.status = PENDING
        self.entry_time = time.time()
        self.exit_time = None
        self.wait_minutes = 0

    def to_dict(self):
        return {
            "id": self.id, "name": self.name, "type": self.type,
            "priority": self.priority, "emergency": self.emergency,
            "status": self.status, "entry_time": self.entry_time,
            "exit_time": self.exit_time, "wait_minutes": self.wait_minutes
        }

    @staticmethod
    def from_dict(data):
        f = Flight(data["id"], data["name"], data["type"], data["priority"], data["emergency"])
        f.status = data["status"]
        f.entry_time = data["entry_time"]
        f.exit_time = data["exit_time"]
        f.wait_minutes = data["wait_minutes"]
        return f

# ---------------- DATA STRUCTURES ----------------
BST = {}
normal_queue = []
emergency_queue = []
history = []

SAVE_FILE = "flights.json"

# ---------------- STORAGE ----------------
def save_data():
    all_data = {
        "bst": {str(k): v.to_dict() for k, v in BST.items()},  # FIXED: str keys for JSON
        "normal_queue": [f.id for f in normal_queue],
        "emergency_queue": [f.id for f in emergency_queue],
        "history": [f.id for f in history]
    }
    try:
        with open(SAVE_FILE, "w") as f:
            json.dump(all_data, f, indent=4)
    except:
        pass

def load_data():
    global BST, normal_queue, emergency_queue, history
    if not os.path.exists(SAVE_FILE):
        return

    try:
        with open(SAVE_FILE, "r") as f:
            data = json.load(f)
        
        BST = {}
        for key, fdata in data["bst"].items():
            BST[int(key)] = Flight.from_dict(fdata)

        normal_queue = [BST[fid] for fid in data.get("normal_queue", []) if fid in BST]
        emergency_queue = [BST[fid] for fid in data.get("emergency_queue", []) if fid in BST]
        history = [BST[fid] for fid in data.get("history", []) if fid in BST]
    except:
        pass

# ---------------- LOGIC ----------------
def add_flight(manual=False):
    global next_id
    if manual:
        try:
            fid = int(id_entry.get())
        except:
            messagebox.showwarning("Error", "Enter valid numeric ID")
            return
        if fid in BST:
            messagebox.showwarning("Duplicate", "Flight ID already exists")
            return
        name = name_entry.get().strip()
        if not name:
            name = f"Flight{fid}"
        ftype = type_var.get()
        priority = priority_var.get()
        emergency_type = emergency_var.get()
    else:
        fid = random.randint(100, 999)
        while fid in BST: 
            fid = random.randint(100, 999)
        airlines = ["Indigo", "AirIndia", "Vistara", "SpiceJet", "GoFirst"]
        name = random.choice(airlines) + str(fid)[-3:]
        ftype = random.choice([LANDING, TAKEOFF])
        priority = EMERGENCY if random.randint(1, 10) <= 2 else NORMAL
        emergency_type = random.choice(EMERGENCY_TYPES[1:]) if priority == EMERGENCY else "None"

    flight = Flight(fid, name, ftype, priority, emergency_type)
    BST[fid] = flight

    if priority == EMERGENCY:
        emergency_queue.append(flight)
        # Sort by emergency priority (Engine > Medical > Fuel)
        emergency_queue.sort(key=lambda x: EMERGENCY_PRIORITY.get(x.emergency, 0), reverse=True)
    else:
        normal_queue.append(flight)

    refresh_ui()
    save_data()
    status_label.config(text=f"✈️ Added Flight {fid} ({priority})", fg="lightgreen")

def process_next():
    global runway_busy
    if runway_busy:
        messagebox.showwarning("Runway", "Runway is busy! Please wait...")
        return
        
    if emergency_queue:
        f = emergency_queue.pop(0)
    elif normal_queue:
        f = normal_queue.pop(0)
    else:
        messagebox.showinfo("Info", "No flights to process")
        return

    # Simulate runway delay
    runway_busy = True
    process_btn.config(state="disabled", text="🔄 PROCESSING...")
    status_label.config(text="🛑 RUNWAY BUSY - Processing...", fg="orange")
    root.update()
    
    # 3 second delay
    root.after(3000, lambda: finish_process(f))

def finish_process(f):
    global runway_busy
    f.status = PROCESSED
    f.exit_time = time.time()
    f.wait_minutes = round((f.exit_time - f.entry_time) / 60, 2)
    history.append(f)
    
    runway_busy = False
    process_btn.config(state="normal", text="🚀 Process Next Flight")
    status_label.config(text=f"✅ Processed {f.id} (Wait: {f.wait_minutes}min)", fg="lightgreen")
    refresh_ui()
    save_data()

def cancel_flight():
    try:
        fid = int(id_entry.get())
    except:
        messagebox.showwarning("Error", "Enter valid ID to cancel")
        return

    if fid not in BST:
        messagebox.showwarning("Not Found", "Flight does not exist")
        return

    f = BST[fid]
    if f.status != PENDING:
        messagebox.showwarning("Cannot Cancel", "Only pending flights can be cancelled")
        return
        
    f.status = CANCELLED
    f.exit_time = time.time()
    f.wait_minutes = round((f.exit_time - f.entry_time) / 60, 2)

    if f in normal_queue: 
        normal_queue.remove(f)
    if f in emergency_queue: 
        emergency_queue.remove(f)
    if f not in history: 
        history.append(f)

    refresh_ui()
    save_data()
    status_label.config(text=f"❌ Cancelled Flight {fid}", fg="red")

def auto_cancel():
    cancelled = 0
    now = time.time()
    for f in list(BST.values()):
        if f.status == PENDING and (now - f.entry_time) / 60 > 5:  # 5 min threshold
            f.status = CANCELLED
            f.exit_time = now
            f.wait_minutes = round((now - f.entry_time) / 60, 2)
            if f in normal_queue: normal_queue.remove(f)
            if f in emergency_queue: emergency_queue.remove(f)
            if f not in history: history.append(f)
            cancelled += 1
    if cancelled:
        status_label.config(text=f"⚠️ Auto-cancelled {cancelled} flights", fg="orange")
        refresh_ui()
        save_data()
    root.after(30000, auto_cancel)  # Check every 30 seconds

# ---------------- ENHANCED UI ----------------
root = tk.Tk()
root.title("🛫 AIRPORT CONTROL TOWER v2.0")
root.geometry("1400x750")
root.configure(bg="#1a1a2e")
root.resizable(True, True)

# Style
style = ttk.Style()
style.theme_use('clam')
style.configure('TButton', font=('Arial', 10, 'bold'))
style.configure('TLabel', background="#1a1a2e", foreground="white")

runway_busy = False

# Title
title_label = tk.Label(root, text="🛫 AIRPORT CONTROL TOWER", font=("Arial", 24, "bold"), 
                      bg="#16213e", fg="#00d4ff")
title_label.pack(pady=10)

# Status Bar
status_label = tk.Label(root, text="🟢 RUNWAY CLEAR - Ready", font=("Arial", 12), 
                       bg="#0f3460", fg="lightgreen", relief=tk.SUNKEN, anchor=tk.W)
status_label.pack(fill=tk.X, pady=(0,10))

# Left Panel - Controls
control_frame = tk.LabelFrame(root, text="🎮 CONTROL PANEL", font=("Arial", 14, "bold"), 
                             bg="#1a1a2e", fg="white", padx=10, pady=10)
control_frame.pack(side=tk.LEFT, fill=tk.Y, padx=(10,5), pady=10)

# Form
tk.Label(control_frame, text="Flight ID:", bg="#1a1a2e", fg="white").pack(anchor=tk.W)
id_entry = tk.Entry(control_frame, font=("Arial", 10)); id_entry.pack(fill=tk.X, pady=(0,5))

tk.Label(control_frame, text="Name:", bg="#1a1a2e", fg="white").pack(anchor=tk.W)
name_entry = tk.Entry(control_frame, font=("Arial", 10)); name_entry.pack(fill=tk.X, pady=(0,5))

tk.Label(control_frame, text="Type:", bg="#1a1a2e", fg="white").pack(anchor=tk.W)
type_var = tk.StringVar(value=LANDING)
tk.OptionMenu(control_frame, type_var, LANDING, TAKEOFF).pack(fill=tk.X, pady=(0,5))

tk.Label(control_frame, text="Priority:", bg="#1a1a2e", fg="white").pack(anchor=tk.W)
priority_var = tk.StringVar(value=NORMAL)
priority_menu = tk.OptionMenu(control_frame, priority_var, NORMAL, EMERGENCY)
priority_menu.pack(fill=tk.X, pady=(0,5))

tk.Label(control_frame, text="Emergency:", bg="#1a1a2e", fg="white").pack(anchor=tk.W)
emergency_var = tk.StringVar(value="None")
emergency_menu = tk.OptionMenu(control_frame, emergency_var, *EMERGENCY_TYPES)
emergency_menu.pack(fill=tk.X, pady=(0,10))

# Buttons
tk.Button(control_frame, text="➕ Add Manual Flight", command=lambda: add_flight(True),
         bg="#00d4ff", fg="white", font=("Arial", 11, "bold"), pady=8).pack(fill=tk.X, pady=2)
tk.Button(control_frame, text="🎲 Add Random Flight", command=lambda: add_flight(False),
         bg="#ff6b6b", fg="white", font=("Arial", 11, "bold"), pady=8).pack(fill=tk.X, pady=2)
process_btn = tk.Button(control_frame, text="🚀 Process Next Flight", command=process_next,
                       bg="#4ecdc4", fg="white", font=("Arial", 11, "bold"), pady=8)
process_btn.pack(fill=tk.X, pady=2)
tk.Button(control_frame, text="❌ Cancel Flight", command=cancel_flight,
         bg="#ff8e8e", fg="white", font=("Arial", 11, "bold"), pady=8).pack(fill=tk.X, pady=2)

# Right Panel - Displays
display_frame = tk.Frame(root, bg="#1a1a2e")
display_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=(5,10), pady=10)

# Main Table
tk.Label(display_frame, text="📊 ALL FLIGHTS", font=("Arial", 14, "bold"), 
        bg="#1a1a2e", fg="#00d4ff").pack(anchor=tk.W)
cols = ("ID", "Name", "Type", "Priority", "Emergency", "Status", "Wait")
table = ttk.Treeview(display_frame, columns=cols, show='headings', height=18)
for col in cols:
    table.heading(col, text=col)
    table.column(col, width=100)
table.pack(fill=tk.BOTH, expand=True, pady=(0,10))

# Queues
queue_frame = tk.Frame(display_frame, bg="#1a1a2e")
queue_frame.pack(fill=tk.X)

tk.Label(queue_frame, text="🚨 EMERGENCY QUEUE", font=("Arial", 12, "bold"), 
        bg="#1a1a2e", fg="#ff4757").pack(side=tk.LEFT, padx=(0,10))
emergency_list = tk.Listbox(queue_frame, width=35, height=8, bg="#2d3436", fg="white", 
                           font=("Courier", 10), selectbackground="#ff4757")
emergency_list.pack(side=tk.LEFT, fill=tk.Y, padx=(0,10))

tk.Label(queue_frame, text="✈️ NORMAL QUEUE", font=("Arial", 12, "bold"), 
        bg="#1a1a2e", fg="#74b9ff").pack(side=tk.LEFT)
normal_list = tk.Listbox(queue_frame, width=35, height=8, bg="#2d3436", fg="white", 
                        font=("Courier", 10), selectbackground="#74b9ff")
normal_list.pack(side=tk.LEFT, fill=tk.Y)

def refresh_ui():
    table.delete(*table.get_children())
    emergency_list.delete(0, tk.END)
    normal_list.delete(0, tk.END)

    for f in sorted(BST.values(), key=lambda x: x.id):
        table.insert("", tk.END, values=(
            f.id, f.name[:15], f.type, f.priority, 
            f.emergency[:12], f.status, f"{f.wait_minutes:.1f}"
        ))

    for f in emergency_queue:
        emergency_list.insert(tk.END, f"🔴 {f.id:3d} {f.name:12s} {f.emergency}")

    for f in normal_queue:
        normal_list.insert(tk.END, f"🔵 {f.id:3d} {f.name:12s} {f.type}")

# Initialize
load_data()
refresh_ui()
auto_cancel()

root.mainloop()
