import os
import tkinter as tk
from tkinter import ttk, messagebox, simpledialog
from PIL import Image, ImageTk, ImageFilter, ImageEnhance
import sqlite3
import datetime
import paho.mqtt.client as mqtt
import random
import re
import threading
import queue

# Constants
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DB_PATH = os.path.join(BASE_DIR, 'device_history.db')
MQTT_BROKER = 'localhost'
MQTT_PORT = 1883
# ESP32 MAC Address for MQTT topics (will be updated dynamically)
ESP32_MAC = 'EC6260C4FD80'
BG_COLOR = '#E8F4FA'
PANEL_BG = '#DFF5FB'
SELECT_BG = '#C8EFFF'
BORDER_COLOR = '#1E4E8C'
BTN_BG = '#1E4E8C'
BTN_FG = 'white'

# Database setup: history and users tables
def init_database():
    """Initialize database - called from main thread"""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS users (
            username TEXT PRIMARY KEY,
            password TEXT
        )
    ''')
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT,
            device_id INTEGER,
            program TEXT,
            parameters TEXT
        )
    ''')
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS device_config (
            id INTEGER PRIMARY KEY,
            mac_address TEXT,
            last_updated TEXT
        )
    ''')
    conn.commit()
    conn.close()

# Initialize database
init_database()

class DatabaseManager:
    """Thread-safe database manager"""
    def __init__(self):
        self.lock = threading.Lock()
    
    def execute_query(self, query, params=None):
        """Execute a query safely"""
        with self.lock:
            conn = sqlite3.connect(DB_PATH)
            cursor = conn.cursor()
            try:
                if params:
                    cursor.execute(query, params)
                else:
                    cursor.execute(query)
                conn.commit()
                result = cursor.fetchall()
                return result
            except Exception as e:
                print(f"Database error: {e}")
                return None
            finally:
                conn.close()
    
    def insert_history(self, timestamp, device_id, program, parameters):
        """Insert history record safely"""
        query = 'INSERT INTO history(timestamp, device_id, program, parameters) VALUES (?, ?, ?, ?)'
        return self.execute_query(query, (timestamp, device_id, program, parameters))
    
    def get_all_history(self):
        """Get all history records safely"""
        query = 'SELECT timestamp, device_id, program, parameters FROM history ORDER BY id DESC'
        return self.execute_query(query)
    
    def get_user(self, username):
        """Get user safely"""
        query = 'SELECT password FROM users WHERE username=?'
        result = self.execute_query(query, (username,))
        return result[0] if result else None
    
    def insert_user(self, username, password):
        """Insert user safely"""
        query = 'INSERT INTO users(username,password) VALUES (?,?)'
        return self.execute_query(query, (username, password))
    
    def get_mac_address(self):
        """Get MAC address safely"""
        query = 'SELECT mac_address FROM device_config WHERE id=1'
        result = self.execute_query(query)
        return result[0][0] if result else None
    
    def update_mac_address(self, mac_address):
        """Update MAC address safely"""
        query = 'UPDATE device_config SET mac_address=?, last_updated=? WHERE id=1'
        timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        return self.execute_query(query, (mac_address, timestamp))
    
    def insert_mac_address(self, mac_address):
        """Insert MAC address safely"""
        query = 'INSERT INTO device_config(id, mac_address, last_updated) VALUES (?, ?, ?)'
        timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        return self.execute_query(query, (1, mac_address, timestamp))

class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title('IC Test System')
        self.geometry('1000x700')
        self.configure(bg=BG_COLOR)

        # Initialize database manager
        self.db_manager = DatabaseManager()
        
        # Create message queue for thread-safe communication
        self.message_queue = queue.Queue()
        
        # Container for multiple pages
        self.container = tk.Frame(self, bg=BG_COLOR)
        self.container.pack(fill='both', expand=True)

        # Create frames
        self.frames = {}
        for F in (LoginPage, MainPage):
            page = F(parent=self.container, controller=self, db_manager=self.db_manager)
            self.frames[F] = page
            page.place(relwidth=1, relheight=1)

        # Show login first
        self.show_frame(LoginPage)
        
        # Start processing messages from MQTT thread
        self.process_messages()

    def show_frame(self, page_class):
        frame = self.frames[page_class]
        frame.tkraise()
    
    def process_messages(self):
        """Process messages from MQTT thread in GUI thread"""
        try:
            while True:
                message = self.message_queue.get_nowait()
                # Process message in main thread
                if hasattr(self.frames[MainPage], 'process_mqtt_message'):
                    self.frames[MainPage].process_mqtt_message(message)
        except queue.Empty:
            pass
        # Schedule next check
        self.after(100, self.process_messages)

class LoginPage(tk.Frame):
    def __init__(self, parent, controller, db_manager):
        super().__init__(parent, bg=BG_COLOR)
        self.controller = controller
        self.db_manager = db_manager

        # Background image with blur and balance effects
        bg_path = os.path.join(BASE_DIR, 'background.png')
        if os.path.exists(bg_path):
            try:
                # Load and process background image
                original_img = Image.open(bg_path)
                
                # Resize to fit screen while maintaining aspect ratio
                img_width, img_height = original_img.size
                screen_ratio = 1000 / 700
                img_ratio = img_width / img_height
                
                if img_ratio > screen_ratio:
                    # Image is wider, fit to height
                    new_height = 700
                    new_width = int(new_height * img_ratio)
                else:
                    # Image is taller, fit to width
                    new_width = 1000
                    new_height = int(new_width / img_ratio)
                
                # Resize and crop to center
                resized_img = original_img.resize((new_width, new_height), Image.Resampling.LANCZOS)
                
                # Crop to center if needed
                if new_width > 1000 or new_height > 700:
                    left = (new_width - 1000) // 2
                    top = (new_height - 700) // 2
                    resized_img = resized_img.crop((left, top, left + 1000, top + 700))
                
                # Apply blur effect
                blurred_img = resized_img.filter(ImageFilter.GaussianBlur(radius=3))
                
                # Adjust brightness and contrast for better balance
                enhancer = ImageEnhance.Brightness(blurred_img)
                balanced_img = enhancer.enhance(0.7)  # Darken slightly
                
                enhancer = ImageEnhance.Contrast(balanced_img)
                balanced_img = enhancer.enhance(1.2)  # Increase contrast slightly
                
                self.bg_photo = ImageTk.PhotoImage(balanced_img)
                bg_label = tk.Label(self, image=self.bg_photo)
                bg_label.place(x=0, y=0, relwidth=1, relheight=1)
                
            except Exception as e:
                print(f"Error loading background image: {e}")
                # Fallback to solid color background
                self.configure(bg=BG_COLOR)

        # Header section with solid background (optimized height)
        header_section = tk.Frame(self, bg='white', relief='flat', bd=0)
        header_section.pack(fill='x', padx=0, pady=0)
        
        # Add subtle shadow under header
        shadow_header = tk.Frame(self, bg='gray85', height=2)
        shadow_header.pack(fill='x')
        
        # Inner header container with reduced padding for better balance
        header_container = tk.Frame(header_section, bg='white')
        header_container.pack(fill='x', padx=20, pady=10)  # Reduced from pady=15
        
        # Configure grid weights for header container
        header_container.grid_columnconfigure(0, weight=1)  # Logo section
        header_container.grid_columnconfigure(1, weight=2)  # Text section
        
        # LEFT SIDE: Logo section
        logo_frame = tk.Frame(header_container, bg='white')
        logo_frame.grid(row=0, column=0, sticky='w', padx=(0, 20))
        
        # List of logo files to look for
        logo_files = ['SPKT.png', 'LOGO_LAB.jpg']
        logos_loaded = 0
        logo_height = 90  # Fixed height for both logos, width will vary
        
        for logo_file in logo_files:
            logo_path = os.path.join(BASE_DIR, logo_file)
            if os.path.exists(logo_path):
                try:
                    logo_img = Image.open(logo_path)
                    
                    # Calculate width based on original aspect ratio
                    original_width, original_height = logo_img.size
                    aspect_ratio = original_width / original_height
                    new_width = int(logo_height * aspect_ratio)
                    
                    # Resize maintaining aspect ratio with fixed height
                    logo_img = logo_img.resize((new_width, logo_height), Image.Resampling.LANCZOS)
                    logo_photo = ImageTk.PhotoImage(logo_img)
                    
                    # Create logo container with dynamic width
                    logo_container = tk.Frame(logo_frame, bg='white', relief='solid', bd=1, 
                                            width=new_width+10, height=logo_height+10)
                    logo_container.pack(side='left', padx=8, pady=3)
                    logo_container.pack_propagate(False)  # Maintain fixed size
                    
                    logo_label = tk.Label(logo_container, image=logo_photo, bg='white')
                    logo_label.pack(expand=True)
                    
                    # Keep reference to prevent garbage collection
                    setattr(self, f'logo_photo_{logos_loaded}', logo_photo)
                    logos_loaded += 1
                    
                except Exception as e:
                    print(f"Error loading logo {logo_file}: {e}")
        
        # If no logos loaded, show placeholders
        if logos_loaded == 0:
            for i in range(2):
                placeholder_logo = tk.Frame(logo_frame, bg='lightgray', width=logo_height+10, 
                                          height=logo_height+10, relief='solid', bd=1)
                placeholder_logo.pack(side='left', padx=8, pady=3)
                placeholder_logo.pack_propagate(False)
                tk.Label(placeholder_logo, text=f'LOGO {i+1}', bg='lightgray', fg='gray', 
                         font=('Arial', 12, 'bold')).pack(expand=True)
        
        # RIGHT SIDE: Project information section (repositioned)
        info_frame = tk.Frame(header_container, bg='white')
        info_frame.grid(row=0, column=1, sticky='ne', padx=(20, 0))  # Changed to 'ne' for top-right
        
        # Project title and information with compact spacing
        tk.Label(info_frame, text='GRADUATION PROJECT', bg='white', fg=BORDER_COLOR,
                 font=('Arial', 16, 'bold')).pack(anchor='e', pady=(5, 2))  # Reduced font and spacing
        
        tk.Label(info_frame, text='IC TESTING MACHINE', bg='white', fg='#2C3E50',
                 font=('Arial', 14, 'bold')).pack(anchor='e', pady=(0, 8))  # Reduced font and spacing
        
        # Student information section
        students_frame = tk.Frame(info_frame, bg='white')
        students_frame.pack(anchor='e')
        
        # Student 1
        student1_frame = tk.Frame(students_frame, bg='white')
        student1_frame.pack(anchor='e', pady=1)  # Reduced spacing
        
        tk.Label(student1_frame, text='Trần Thành Đăng', bg='white', fg='#34495E',
                 font=('Arial', 11, 'bold')).pack(side='left')  # Reduced font
        tk.Label(student1_frame, text=' - ', bg='white', fg='#7F8C8D',
                 font=('Arial', 11)).pack(side='left')
        tk.Label(student1_frame, text='21151217', bg='white', fg='#7F8C8D',
                 font=('Arial', 11)).pack(side='left')
        
        # Student 2
        student2_frame = tk.Frame(students_frame, bg='white')
        student2_frame.pack(anchor='e', pady=1)  # Reduced spacing
        
        tk.Label(student2_frame, text='Bùi Dương Quốc Bảo', bg='white', fg='#34495E',
                 font=('Arial', 11, 'bold')).pack(side='left')  # Reduced font
        tk.Label(student2_frame, text=' - ', bg='white', fg='#7F8C8D',
                 font=('Arial', 11)).pack(side='left')
        tk.Label(student2_frame, text='21151195', bg='white', fg='#7F8C8D',
                 font=('Arial', 11)).pack(side='left')

        # Login box with enhanced styling (adjusted position)
        login_box = tk.Frame(self, bg='white', bd=0, relief='flat')
        login_box.place(relx=0.5, rely=0.62, anchor='center', width=380, height=350)  # Moved up slightly
        
        # Add shadow effect to login box
        shadow_frame = tk.Frame(self, bg='gray70', bd=0)
        shadow_frame.place(relx=0.5, rely=0.62, anchor='center', width=385, height=355)  # Adjusted position
        login_box.lift()  # Bring login box to front
        
        # Add rounded corner effect with inner frame
        inner_frame = tk.Frame(login_box, bg='white', bd=2, relief='groove')
        inner_frame.pack(fill='both', expand=True, padx=5, pady=5)

        tk.Label(inner_frame, text='Please Log In', bg='white', fg=BORDER_COLOR,
                 font=('Arial',18,'bold')).pack(pady=(25,15))

        # Username entry with enhanced styling
        tk.Label(inner_frame, text='Username:', bg='white', fg='black', 
                 font=('Arial',11,'bold')).pack(anchor='w', padx=25, pady=(15,5))
        username_frame = tk.Frame(inner_frame, bg='white')
        username_frame.pack(fill='x', padx=25, pady=(0,15))
        self.username_entry = tk.Entry(username_frame, font=('Arial',12), relief='solid', bd=1)
        self.username_entry.pack(fill='x', ipady=5)

        # Password entry with enhanced styling
        tk.Label(inner_frame, text='Password:', bg='white', fg='black', 
                 font=('Arial',11,'bold')).pack(anchor='w', padx=25, pady=(10,5))
        password_frame = tk.Frame(inner_frame, bg='white')
        password_frame.pack(fill='x', padx=25, pady=(0,20))
        self.password_entry = tk.Entry(password_frame, show='*', font=('Arial',12), relief='solid', bd=1)
        self.password_entry.pack(fill='x', ipady=5)

        # Login and Register buttons with enhanced styling
        btn_frame = tk.Frame(inner_frame, bg='white')
        btn_frame.pack(pady=15)
        
        login_btn = tk.Button(btn_frame, text='Login', bg=BTN_BG, fg=BTN_FG, bd=0, width=12,
                             font=('Arial',11,'bold'), relief='flat', command=self.check_credentials)
        login_btn.pack(side='left', padx=8)
        
        register_btn = tk.Button(btn_frame, text='Register', bg='#6C757D', fg='white', bd=0, width=12,
                                font=('Arial',11,'bold'), relief='flat', command=self.open_register_dialog)
        register_btn.pack(side='left', padx=8)

        self.error_label = tk.Label(inner_frame, text='', bg='white', fg='red', font=('Arial',10))
        self.error_label.pack(pady=(10,20))

    def check_credentials(self):
        username = self.username_entry.get().strip()
        password = self.password_entry.get().strip()
        if not username or not password:
            self.error_label.config(text='Enter both username and password')
            return
        
        user_data = self.db_manager.get_user(username)
        if user_data and user_data[0] == password:
            self.error_label.config(text='')
            self.controller.show_frame(MainPage)
        else:
            self.error_label.config(text='Invalid username or password')

    def open_register_dialog(self):
        dialog = tk.Toplevel(self)
        dialog.title('Register')
        dialog.configure(bg=BG_COLOR)

        tk.Label(dialog, text='Register New Account', bg=BG_COLOR, fg=BORDER_COLOR,
                 font=('Arial',14,'bold')).pack(pady=(10,10))
        tk.Label(dialog, text='Username:', bg=BG_COLOR, fg='black', font=('Arial',10)).pack(anchor='w', padx=20)
        user_ent = tk.Entry(dialog, font=('Arial',12))
        user_ent.pack(fill='x', padx=20, pady=(0,10))
        tk.Label(dialog, text='Password:', bg=BG_COLOR, fg='black', font=('Arial',10)).pack(anchor='w', padx=20)
        pass_ent = tk.Entry(dialog, show='*', font=('Arial',12))
        pass_ent.pack(fill='x', padx=20, pady=(0,10))
        message = tk.Label(dialog, text='', bg=BG_COLOR, fg='red')
        message.pack(pady=(0,10))

        def register():
            u = user_ent.get().strip()
            p = pass_ent.get().strip()
            if not u or not p:
                message.config(text='Enter both fields')
                return
            try:
                self.db_manager.insert_user(u, p)
                messagebox.showinfo('Success', 'Account registered')
                dialog.destroy()
            except Exception as e:
                if 'UNIQUE constraint failed' in str(e):
                    message.config(text='Username already exists')
                else:
                    message.config(text='Registration failed')

        tk.Button(dialog, text='Submit', bg=BTN_BG, fg=BTN_FG, bd=0, width=10, command=register).pack(pady=10)

class MainPage(tk.Frame):
    def __init__(self, parent, controller, db_manager):
        super().__init__(parent, bg=BG_COLOR)
        self.controller = controller
        self.db_manager = db_manager

        # Initialize WiFi status and program data variables
        self.wifi_status = tk.StringVar(value='Disconnected')
        self.mac_address = tk.StringVar(value=ESP32_MAC)
        
        # Store program data (will be populated from MQTT) - REMOVED 'mode'
        self.program_data = {
            1: {'name': 'Program 1', 'ic_test': '', 'ic_count': ''},
            2: {'name': 'Program 2', 'ic_test': '', 'ic_count': ''},
            3: {'name': 'Program 3', 'ic_test': '', 'ic_count': ''},
            4: {'name': 'Program 4', 'ic_test': '', 'ic_count': ''}
        }
        self.selected_program = None
        
        # NEW: Store available IC names from NameIC topic
        self.available_ic_names = []
        
        # WiFi status tracking - IMPROVED
        self.wifi_timeout_id = None
        self.wifi_check_id = None

        # Header
        header = tk.Frame(self, bg=BORDER_COLOR, height=60)
        header.pack(fill='x')
        header.pack_propagate(False)
        tk.Label(header, text='IC Test System', bg=BORDER_COLOR, fg='white', font=('Arial',16,'bold')).pack(side='left', padx=20)
        tk.Button(header, text='Watch History', bg='white', fg=BORDER_COLOR, bd=0,
                  command=self.open_history_window).pack(side='right', padx=20)
        self.time_label = tk.Label(header, bg=BORDER_COLOR, fg='white', font=('Arial',10))
        self.time_label.pack(side='right')
        self.update_time()

        # Main container below header
        container = tk.Frame(self, bg=BG_COLOR)
        container.pack(fill='both', expand=True, padx=20, pady=(10,20))

        # WiFi Status section
        wifi_section = tk.LabelFrame(container, text='WiFi & Device Status', bg=BG_COLOR, fg=BORDER_COLOR,
                                     font=('Arial',14,'bold'), bd=2, relief='groove')
        wifi_section.pack(fill='x', pady=(0,20))
        
        wifi_frame = tk.Frame(wifi_section, bg=BG_COLOR)
        wifi_frame.pack(fill='x', padx=10, pady=10)
        wifi_frame.columnconfigure(0, weight=1)
        wifi_frame.columnconfigure(1, weight=1)
        wifi_frame.columnconfigure(2, weight=1)

        # WiFi Connection Status
        wifi_status_frame = tk.Frame(wifi_frame, bg=BG_COLOR)
        wifi_status_frame.grid(row=0, column=0, sticky='ew', padx=5)
        
        tk.Label(wifi_status_frame, text='ESP32 WiFi Status:', bg=BG_COLOR, fg='black',
                 font=('Arial',10,'bold')).pack(anchor='w')
        self.wifi_status_label = tk.Label(wifi_status_frame, textvariable=self.wifi_status,
                                         bg=BG_COLOR, fg='red', font=('Arial',12,'bold'))
        self.wifi_status_label.pack(anchor='w', pady=(2,0))

        # MAC Address Display
        mac_frame = tk.Frame(wifi_frame, bg=BG_COLOR)
        mac_frame.grid(row=0, column=1, sticky='ew', padx=5)
        
        tk.Label(mac_frame, text='ESP32 MAC Address:', bg=BG_COLOR, fg='black',
                 font=('Arial',10,'bold')).pack(anchor='w')
        tk.Label(mac_frame, textvariable=self.mac_address, bg=BG_COLOR, fg='black',
                 font=('Arial',12,'bold')).pack(anchor='w', pady=(2,0))

        # Control Button
        btn_frame = tk.Frame(wifi_frame, bg=BG_COLOR)
        btn_frame.grid(row=0, column=2, sticky='ew', padx=5)
        
        tk.Button(btn_frame, text='Change MAC ID', bg=BTN_BG, fg=BTN_FG, bd=0, width=14,
                  command=self.change_mac_address).pack(anchor='e')

        # Program selection panels
        prog_frame = tk.Frame(container, bg=BG_COLOR)
        prog_frame.pack(fill='x', pady=(0,20))
        prog_frame.columnconfigure(0, weight=1)
        prog_frame.columnconfigure(1, weight=1)

        # Available Programs panel
        avail_frame = tk.LabelFrame(prog_frame, text='Available Programs', bg=BG_COLOR, fg=BORDER_COLOR,
                                    font=('Arial',12,'bold'), bd=2, relief='groove')
        avail_frame.grid(row=0, column=0, sticky='nsew', padx=(0,10))
        inner_avail = tk.Frame(avail_frame, bg=BG_COLOR)
        inner_avail.pack(fill='both', expand=True, padx=5, pady=5)

        self.programs = [f'Program {i}' for i in range(1,5)]
        self.prog_labels = []
        for i, prog in enumerate(self.programs, 1):
            lbl = tk.Label(inner_avail, text=prog, bg=BG_COLOR, fg='black', font=('Arial',12))
            lbl.pack(fill='x', padx=10, pady=6)
            lbl.bind('<Button-1>', self._make_select_handler(i, lbl))
            self.prog_labels.append(lbl)

        # Program Parameters panel - REMOVED 'Mode'
        param_frame = tk.LabelFrame(prog_frame, text='Program Parameters', bg=BG_COLOR, fg=BORDER_COLOR,
                                    font=('Arial',12,'bold'), bd=2, relief='groove')
        param_frame.grid(row=0, column=1, sticky='nsew', padx=(10,0))
        inner_param = tk.Frame(param_frame, bg=BG_COLOR)
        inner_param.pack(fill='both', expand=True, padx=5, pady=5)

        self.info_vars = {}
        for field in ['Program Name','IC Test Name','IC Count']:  # REMOVED 'Mode'
            row = tk.Frame(inner_param, bg=BG_COLOR)
            row.pack(fill='x', padx=10, pady=6)
            tk.Label(row, text=f'{field}:', width=15, bg=BG_COLOR, fg='black',
                     font=('Arial',10,'bold')).pack(side='left')
            var = tk.StringVar(value='—')
            tk.Label(row, textvariable=var, bg=BG_COLOR, fg='black', font=('Arial',10)).pack(side='left')
            self.info_vars[field] = var

        tk.Button(inner_param, text='Modify', bg=BTN_BG, fg=BTN_FG, bd=0, width=16,
                  command=self.open_modify_dialog).pack(anchor='e', pady=(10,0))

        # Device Status section
        status_section = tk.LabelFrame(container, text='Device Status', bg=BG_COLOR, fg=BORDER_COLOR,
                                       font=('Arial',14,'bold'), bd=2, relief='groove')
        status_section.pack(fill='both', expand=True)
        status_frame = tk.Frame(status_section, bg=BG_COLOR)
        status_frame.pack(fill='both', expand=True, padx=10, pady=10)
        for i in range(3):
            status_frame.columnconfigure(i, weight=1)

        self.device_cards = {}
        for i in range(1,4):
            df = tk.LabelFrame(status_frame, text=f'Device {i}', bg=BG_COLOR, fg=BORDER_COLOR,
                                font=('Arial',12,'bold'), bd=1, relief='solid')
            df.grid(row=0, column=i-1, padx=10, sticky='nsew')
            self.device_cards[i] = {}
            for label in ['State','Program','IC Count']:
                row = tk.Frame(df, bg=BG_COLOR)
                row.pack(fill='x', padx=10, pady=6)
                tk.Label(row, text=f'{label}:', width=12, bg=BG_COLOR, fg='black', font=('Arial',10,'bold')).pack(side='left')
                var = tk.StringVar(value='—')
                lbl = tk.Label(row, textvariable=var, bg=BG_COLOR, font=('Arial',10), anchor='w')
                lbl.pack(side='left', fill='x')
                self.device_cards[i][label.lower()] = (var, lbl)

        # MQTT setup
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.client.connect(MQTT_BROKER, MQTT_PORT, 60)
        self.client.loop_start()

        # Load existing MAC address from database
        self.load_mac_address()
        
        # Request initial program data from ESP32
        self.after(1000, self.request_program_data)
        
        # NEW: Request available IC names
        self.after(1500, self.request_ic_names)

    def load_mac_address(self):
        """Load MAC address from database"""
        try:
            mac = self.db_manager.get_mac_address()
            if mac:
                self.mac_address.set(mac)
            else:
                # Use the predefined ESP32 MAC
                self.mac_address.set(ESP32_MAC)
                self.db_manager.insert_mac_address(ESP32_MAC)
        except Exception as e:
            print(f"Error loading MAC address: {e}")

    def request_program_data(self):
        """Request current program data from ESP32"""
        current_mac = self.mac_address.get()
        # Request data for all 4 programs
        for i in range(1, 5):
            self.client.publish(f'{current_mac}/DataProgram/{i}/request', 'get')

    def request_ic_names(self):
        """NEW: Request available IC names from ESP32"""
        current_mac = self.mac_address.get()
        self.client.publish(f'{current_mac}/NameIC/request', 'get')
        print(f"Requested IC names from: {current_mac}/NameIC")

    def request_wifi_status(self):
        """Request WiFi connection status from ESP32 with improved timing"""
        current_mac = self.mac_address.get()
        
        # Cancel any existing timeouts
        if self.wifi_timeout_id:
            self.after_cancel(self.wifi_timeout_id)
        if self.wifi_check_id:
            self.after_cancel(self.wifi_check_id)
        
        # Keep current status until we get a response (don't show "Checking...")
        print(f"Published WiFi status request to: {current_mac}/checkconnect")
        
        # Publish status request
        self.client.publish(f'{current_mac}/checkconnect', 'status')
        
        # Set timeout for no response (8 seconds)
        self.wifi_timeout_id = self.after(8000, self.wifi_timeout)
        
        # Schedule next status check in 15 seconds
        self.wifi_check_id = self.after(15000, self.request_wifi_status)

    def wifi_timeout(self):
        """Handle WiFi status timeout"""
        print("WiFi status check timed out")
        self.wifi_status.set('No Response')
        self.wifi_status_label.config(fg='gray')
        self.wifi_timeout_id = None

    def change_mac_address(self):
        """Open dialog to change MAC address and update all MQTT topics"""
        current_mac = self.mac_address.get()
        new_mac = simpledialog.askstring(
            "Change MAC Address",
            f"Current MAC: {current_mac}\n\nEnter new MAC address (format: XXXXXXXXXXXX or XX:XX:XX:XX:XX:XX):",
            initialvalue=current_mac
        )
        
        if new_mac:
            # Clean the MAC address (remove colons and convert to uppercase)
            clean_mac = new_mac.replace(':', '').replace('-', '').upper()
            
            # Validate MAC address format (12 hex characters)
            mac_pattern = r'^[0-9A-F]{12}$'
            if re.match(mac_pattern, clean_mac):
                # Store old MAC for unsubscribing
                old_mac = self.mac_address.get()
                
                # Update the MAC address
                self.mac_address.set(clean_mac)
                
                # Save to database
                self.db_manager.update_mac_address(clean_mac)
                
                # Unsubscribe from old MAC topics
                self.client.unsubscribe(f'{old_mac}/WiFiStatus')
                self.client.unsubscribe(f'{old_mac}/status')  # Fixed: listen for status response
                self.client.unsubscribe(f'{old_mac}/NameIC')  # NEW: NameIC topic
                for i in range(1, 5):
                    self.client.unsubscribe(f'{old_mac}/DataProgram/{i}')
                for i in range(1, 4):
                    self.client.unsubscribe(f'{old_mac}/StatusDevice/{i}')
                # History topic without device number
                self.client.unsubscribe(f'{old_mac}/History')
                
                # Subscribe to new MAC topics
                self.client.subscribe(f'{clean_mac}/WiFiStatus')
                self.client.subscribe(f'{clean_mac}/status')  # Fixed: listen for status response
                self.client.subscribe(f'{clean_mac}/NameIC')  # NEW: NameIC topic
                for i in range(1, 5):
                    self.client.subscribe(f'{clean_mac}/DataProgram/{i}')
                for i in range(1, 4):
                    self.client.subscribe(f'{clean_mac}/StatusDevice/{i}')
                # History topic without device number
                self.client.subscribe(f'{clean_mac}/History')
                
                # Publish new MAC to notify ESP32
                self.client.publish(f'{clean_mac}/MacID', clean_mac)
                
                # Reset WiFi status and request new status
                self.wifi_status.set('Disconnected')
                self.wifi_status_label.config(fg='red')
                self.after(2000, self.request_wifi_status)  # Wait 2 seconds before first check
                
                # Request program data with new MAC
                self.request_program_data()
                
                # NEW: Request IC names with new MAC
                self.request_ic_names()
                
                messagebox.showinfo("Success", f"MAC address updated to: {clean_mac}\nMQTT topics updated automatically!")
            else:
                messagebox.showerror("Invalid Format", "Please enter MAC address in format: XXXXXXXXXXXX or XX:XX:XX:XX:XX:XX")

    def update_time(self):
        now = datetime.datetime.now().strftime('%H:%M:%S   %B %d, %Y')
        self.time_label.config(text=now)
        self.after(1000, self.update_time)

    def _make_select_handler(self, prog_num, label):
        def handler(evt):
            for lbl in self.prog_labels:
                lbl.config(bg=BG_COLOR)
            label.config(bg=SELECT_BG)
            self.on_program_select(prog_num)
        return handler

    def on_program_select(self, prog_num):
        """Handle program selection and update parameter display - REMOVED Mode"""
        self.selected_program = prog_num
        prog_data = self.program_data[prog_num]
        
        self.info_vars['Program Name'].set(prog_data['name'])
        self.info_vars['IC Test Name'].set(prog_data['ic_test'])
        self.info_vars['IC Count'].set(prog_data['ic_count'])

    def on_connect(self, client, userdata, flags, rc):
        """Subscribe to MQTT topics using current MAC address"""
        current_mac = self.mac_address.get()
        print(f"MQTT Connected! Subscribing to topics with MAC: {current_mac}")
        
        # Subscribe to WiFi status topics
        client.subscribe(f'{current_mac}/WiFiStatus')
        client.subscribe(f'{current_mac}/status')  # Fixed: listen for status response
        
        # NEW: Subscribe to NameIC topic
        client.subscribe(f'{current_mac}/NameIC')
        
        # Subscribe to all DataProgram topics (1-4)
        for i in range(1, 5):
            client.subscribe(f'{current_mac}/DataProgram/{i}')
        
        # Subscribe to all StatusDevice topics (1-3)
        for i in range(1, 4):
            client.subscribe(f'{current_mac}/StatusDevice/{i}')
            
        # Subscribe to History topic (NO device number - single topic for all devices)
        client.subscribe(f'{current_mac}/History')
        
        print(f"Subscribed to topics:")
        print(f"  - {current_mac}/WiFiStatus")
        print(f"  - {current_mac}/status")
        print(f"  - {current_mac}/NameIC")  # NEW
        print(f"  - {current_mac}/DataProgram/1-4")
        print(f"  - {current_mac}/StatusDevice/1-3") 
        print(f"  - {current_mac}/History")
        
        # Start requesting WiFi status with initial delay
        self.after(3000, self.request_wifi_status)  # Initial delay of 3 seconds

    def on_message(self, client, userdata, msg):
        """Handle MQTT messages - Queue them for main thread processing"""
        topic = msg.topic
        payload = msg.payload.decode()
        
        print(f"MQTT received: {topic} -> {payload}")
        
        # Queue the message for processing in main thread
        message = {
            'topic': topic,
            'payload': payload,
            'timestamp': datetime.datetime.now()
        }
        self.controller.message_queue.put(message)

    def parse_date_time(self, date_str, time_str):
        """
        FIXED: Parse date and time strings correctly for sorting
        Input: date_str = "17/6/25" (DD/M/YY), time_str = "3:20:13" (H:MM:SS)
        Output: datetime object for proper sorting
        """
        try:
            # Parse date: 17/6/25 -> day=17, month=6, year=2025
            date_parts = date_str.split('/')
            if len(date_parts) == 3:
                day = int(date_parts[0])
                month = int(date_parts[1])
                year_short = int(date_parts[2])
                
                # Convert 2-digit year to 4-digit (assuming 20xx)
                if year_short < 50:  # Years 00-49 -> 2000-2049
                    year = 2000 + year_short
                else:  # Years 50-99 -> 1950-1999
                    year = 1900 + year_short
                
                # Parse time: 3:20:13
                time_parts = time_str.split(':')
                if len(time_parts) == 3:
                    hour = int(time_parts[0])
                    minute = int(time_parts[1])
                    second = int(time_parts[2])
                    
                    # Create datetime object
                    return datetime.datetime(year, month, day, hour, minute, second)
            
            # Fallback: return current time if parsing fails
            return datetime.datetime.now()
            
        except Exception as e:
            print(f"Error parsing date/time '{date_str} {time_str}': {e}")
            return datetime.datetime.now()

    def process_mqtt_message(self, message):
        """Process MQTT message in main thread (thread-safe)"""
        topic = message['topic']
        payload = message['payload']
        current_mac = self.mac_address.get()
        
        try:
            # Fixed: Handle both WiFiStatus and status topics
            if topic == f'{current_mac}/WiFiStatus' or topic == f'{current_mac}/status':
                # Handle ESP32 WiFi connection status
                # Cancel timeout since we got a response
                if self.wifi_timeout_id:
                    self.after_cancel(self.wifi_timeout_id)
                    self.wifi_timeout_id = None
                
                status = payload.strip().lower()
                if status in ['connected', '1', 'true', 'online']:
                    self.wifi_status.set('Connected')
                    self.wifi_status_label.config(fg='green')
                else:
                    self.wifi_status.set('Disconnected')
                    self.wifi_status_label.config(fg='red')
            
            # NEW: Handle NameIC topic
            elif topic == f'{current_mac}/NameIC':
                # Handle IC names list
                # Expected format: "name1,name2,name3,..."
                if payload.strip():
                    self.available_ic_names = [name.strip() for name in payload.split(',') if name.strip()]
                    print(f"Updated available IC names: {self.available_ic_names}")
                else:
                    self.available_ic_names = []
                    print("No IC names available")
            
            elif topic.startswith(f'{current_mac}/DataProgram/'):
                # Handle DataProgram updates - REMOVED mode
                # Topic format: MAC/DataProgram/N
                # Payload format: "program_name,ic_test_name,ic_count"
                prog_num = int(topic.split('/')[-1])
                if 1 <= prog_num <= 4:
                    parts = payload.split(',')
                    if len(parts) >= 3:
                        self.program_data[prog_num] = {
                            'name': parts[0],
                            'ic_test': parts[1],
                            'ic_count': parts[2]
                        }
                        # Update the program label
                        if prog_num <= len(self.prog_labels):
                            self.prog_labels[prog_num-1].config(text=parts[0])
                        
                        # If this program is currently selected, update the display
                        if self.selected_program == prog_num:
                            self.on_program_select(prog_num)
            
            elif topic.startswith(f'{current_mac}/StatusDevice/'):
                # Handle StatusDevice updates
                # Topic format: MAC/StatusDevice/N
                # Payload format: "status,program_name,ic_count"
                device_num = int(topic.split('/')[-1])
                if 1 <= device_num <= 3:
                    parts = payload.split(',')
                    if len(parts) >= 3:
                        status = parts[0]
                        program = parts[1]
                        ic_count = parts[2]
                        
                        # Update device status display
                        var, lbl = self.device_cards[device_num]['state']
                        var.set(status)
                        lbl.config(fg='green' if status.lower() == 'running' else 'red')
                        
                        # Update program info
                        var, lbl = self.device_cards[device_num]['program']
                        var.set(program)
                        
                        # Update IC count
                        var, lbl = self.device_cards[device_num]['ic count']
                        var.set(ic_count)
            
            elif topic == f'{current_mac}/History':
                # Handle History updates with FIXED date parsing - THREAD SAFE
                # Expected format: "date;time;device_test;ic_name;ic_test_count;result_testing"
                # Example: "17/6/25;3:20:13;IC TESTER 1;744051;2;IC1: Self-test result: failed | IC2: Self-test result: failed"
                
                print(f"Processing history data: {payload}")
                parts = payload.split(';')
                
                if len(parts) >= 6:
                    try:
                        date_part = parts[0]            # 17/6/25
                        time_part = parts[1]            # 3:20:13
                        device_test = parts[2]          # IC TESTER 1
                        ic_name = parts[3]              # 744051
                        ic_test_count = parts[4]        # 2
                        result_testing = ';'.join(parts[5:])  # Remaining text (may contain semicolons)
                        
                        # FIXED: Use the proper date format for storage
                        # Convert the date/time to a format that can be parsed later
                        timestamp = f"{date_part} {time_part}"
                        
                        # Store in database using thread-safe method (device_id set to 0 since not used)
                        self.db_manager.insert_history(timestamp, 0, device_test, 
                                                     f"{ic_name};{ic_test_count};{result_testing}")
                        print(f"History saved: Time: {timestamp}, Device: {device_test}, IC: {ic_name}")
                        
                    except Exception as e:
                        print(f"Error parsing history data: {e}")
                        print(f"Raw data: {payload}")
                else:
                    print(f"Invalid history format - expected 6+ parts, got {len(parts)}: {payload}")
                        
        except Exception as e:
            print(f"Error processing MQTT message {topic}: {e}")
            print(f"Payload: {payload}")

    def open_modify_dialog(self):
        """UPDATED: Open modify dialog with new constraints"""
        if self.selected_program is None:
            messagebox.showwarning("No Selection", "Please select a program first")
            return
            
        prog_num = self.selected_program
        prog_data = self.program_data[prog_num]
        
        dialog = tk.Toplevel(self)
        dialog.title(f'Modify Program {prog_num}')
        dialog.configure(bg=BG_COLOR)
        dialog.geometry('450x250')  # Increased width for combobox
        
        # Character counter for program name
        char_count_var = tk.StringVar()
        
        # Program Name (limited to 12 characters)
        tk.Label(dialog, text='Program Name (max 12 chars):', bg=BG_COLOR, fg='black', 
                font=('Arial',10,'bold')).grid(row=0, column=0, pady=8, padx=15, sticky='e')
        
        name_frame = tk.Frame(dialog, bg=BG_COLOR)
        name_frame.grid(row=0, column=1, pady=8, padx=15, sticky='w')
        
        name_entry = tk.Entry(name_frame, font=('Arial',12), width=15)
        name_entry.pack(side='left')
        name_entry.insert(0, prog_data['name'][:12])  # Limit to 12 chars
        
        char_label = tk.Label(name_frame, textvariable=char_count_var, bg=BG_COLOR, 
                             fg='gray', font=('Arial',9))
        char_label.pack(side='left', padx=(5,0))
        
        def update_char_count(event=None):
            current_text = name_entry.get()
            if len(current_text) > 12:
                name_entry.delete(12, tk.END)
                current_text = name_entry.get()
            char_count_var.set(f"{len(current_text)}/12")
            char_label.config(fg='red' if len(current_text) > 10 else 'gray')
        
        name_entry.bind('<KeyRelease>', update_char_count)
        update_char_count()  # Initial count
        
        # IC Test Name (dropdown from available IC names)
        tk.Label(dialog, text='IC Test Name:', bg=BG_COLOR, fg='black', 
                font=('Arial',10,'bold')).grid(row=1, column=0, pady=8, padx=15, sticky='e')
        
        ic_combo = ttk.Combobox(dialog, font=('Arial',12), width=18, state='readonly')
        ic_combo.grid(row=1, column=1, pady=8, padx=15, sticky='w')
        
        # Populate combobox with available IC names
        if self.available_ic_names:
            ic_combo['values'] = self.available_ic_names
            # Set current value if it exists in the list
            if prog_data['ic_test'] in self.available_ic_names:
                ic_combo.set(prog_data['ic_test'])
            elif self.available_ic_names:
                ic_combo.set(self.available_ic_names[0])  # Default to first option
        else:
            ic_combo['values'] = ['No IC names available']
            ic_combo.set('No IC names available')
        
        # IC Count (restricted to 1-5)
        tk.Label(dialog, text='IC Count (1-5):', bg=BG_COLOR, fg='black', 
                font=('Arial',10,'bold')).grid(row=2, column=0, pady=8, padx=15, sticky='e')
        
        count_frame = tk.Frame(dialog, bg=BG_COLOR)
        count_frame.grid(row=2, column=1, pady=8, padx=15, sticky='w')
        
        count_var = tk.StringVar()
        count_spinbox = tk.Spinbox(count_frame, from_=1, to=5, width=5, font=('Arial',12),
                                  textvariable=count_var, state='readonly')
        count_spinbox.pack(side='left')
        
        # Set current IC count (ensure it's within 1-5 range)
        try:
            current_count = int(prog_data['ic_count']) if prog_data['ic_count'].isdigit() else 1
            current_count = max(1, min(5, current_count))  # Clamp to 1-5 range
        except:
            current_count = 1
        count_var.set(str(current_count))
        
        tk.Label(count_frame, text='(Number of ICs to test)', bg=BG_COLOR, fg='gray',
                font=('Arial',9)).pack(side='left', padx=(5,0))

        def save():
            # Get new values with validation
            new_name = name_entry.get().strip()
            new_ic_test = ic_combo.get().strip()
            new_ic_count = count_var.get().strip()
            
            # Validate program name
            if not new_name:
                messagebox.showerror("Validation Error", "Program name cannot be empty")
                return
            if len(new_name) > 12:
                messagebox.showerror("Validation Error", "Program name must be 12 characters or less")
                return
            
            # Validate IC test name
            if not new_ic_test or new_ic_test == 'No IC names available':
                messagebox.showerror("Validation Error", "Please select a valid IC test name")
                return
            
            # Validate IC count (already constrained by spinbox, but double-check)
            try:
                count_int = int(new_ic_count)
                if not (1 <= count_int <= 5):
                    messagebox.showerror("Validation Error", "IC count must be between 1 and 5")
                    return
            except ValueError:
                messagebox.showerror("Validation Error", "IC count must be a valid number")
                return
            
            current_mac = self.mac_address.get()
            
            # Publish to MQTT (bidirectional update) - REMOVED mode
            payload = f"{new_name},{new_ic_test},{new_ic_count}"
            topic = f"{current_mac}/DataProgram/{prog_num}"
            self.client.publish(topic, payload)
            
            # Update local data (will also be updated via MQTT callback)
            self.program_data[prog_num] = {
                'name': new_name,
                'ic_test': new_ic_test,
                'ic_count': new_ic_count
            }
            
            # Update UI immediately
            self.prog_labels[prog_num-1].config(text=new_name)
            self.on_program_select(prog_num)
            
            messagebox.showinfo("Success", f"Program {prog_num} updated and published to ESP32")
            dialog.destroy()

        btn_frame = tk.Frame(dialog, bg=BG_COLOR)
        btn_frame.grid(row=3, column=0, columnspan=2, pady=20)
        
        tk.Button(btn_frame, text='Cancel', bg='gray', fg='white', bd=0, width=12,
                 command=dialog.destroy).pack(side='left', padx=10)
        tk.Button(btn_frame, text='Save & Publish', bg=BTN_BG, fg=BTN_FG, bd=0, width=12,
                 command=save).pack(side='right', padx=10)

    def open_history_window(self):
        """Open history window with filtering and sorting features"""
        win = tk.Toplevel(self)
        win.title('History Log')
        win.configure(bg=BG_COLOR)
        win.geometry('1400x700')
        
        # Configure grid weights for the main window
        win.grid_rowconfigure(1, weight=1)
        win.grid_columnconfigure(0, weight=1)
        
        # Filter and Sort Controls Frame (with same styling as modify dialog)
        control_frame = tk.Frame(win, bg=BG_COLOR, bd=2, relief='groove')
        control_frame.grid(row=0, column=0, sticky='ew', padx=10, pady=(10,5))
        control_frame.grid_columnconfigure(1, weight=1)
        control_frame.grid_columnconfigure(3, weight=1)
        
        # Filter Section
        tk.Label(control_frame, text='Filter by IC Name:', bg=BG_COLOR, fg='black', 
                font=('Arial',10,'bold')).grid(row=0, column=0, pady=8, padx=15, sticky='e')
        
        self.ic_filter_var = tk.StringVar()
        ic_filter_entry = tk.Entry(control_frame, textvariable=self.ic_filter_var, font=('Arial',10), width=15)
        ic_filter_entry.grid(row=0, column=1, pady=8, padx=(0,15), sticky='w')
        
        tk.Label(control_frame, text='Filter by Device Test:', bg=BG_COLOR, fg='black', 
                font=('Arial',10,'bold')).grid(row=0, column=2, pady=8, padx=15, sticky='e')
        
        self.device_filter_var = tk.StringVar()
        device_filter_entry = tk.Entry(control_frame, textvariable=self.device_filter_var, font=('Arial',10), width=15)
        device_filter_entry.grid(row=0, column=3, pady=8, padx=(0,15), sticky='w')
        
        # Button Frame for filters and sorting
        btn_control_frame = tk.Frame(control_frame, bg=BG_COLOR)
        btn_control_frame.grid(row=1, column=0, columnspan=4, pady=10)
        
        # Filter Buttons
        tk.Button(btn_control_frame, text='Apply Filter', bg=BTN_BG, fg=BTN_FG, bd=0, width=12,
                 font=('Arial',10,'bold'), command=self.apply_history_filter).pack(side='left', padx=5)
        
        tk.Button(btn_control_frame, text='Clear Filter', bg='#6C757D', fg='white', bd=0, width=12,
                 font=('Arial',10,'bold'), command=self.clear_history_filter).pack(side='left', padx=5)
        
        # Sort Buttons
        tk.Label(btn_control_frame, text='Sort by Time:', bg=BG_COLOR, fg='black', 
                font=('Arial',10,'bold')).pack(side='left', padx=(20,5))
        
        tk.Button(btn_control_frame, text='↑ Newest First', bg='#28A745', fg='white', bd=0, width=12,
                 font=('Arial',10,'bold'), command=lambda: self.sort_history('time_desc')).pack(side='left', padx=2)
        
        tk.Button(btn_control_frame, text='↓ Oldest First', bg='#28A745', fg='white', bd=0, width=12,
                 font=('Arial',10,'bold'), command=lambda: self.sort_history('time_asc')).pack(side='left', padx=2)
        
        tk.Label(btn_control_frame, text='Sort by IC Count:', bg=BG_COLOR, fg='black', 
                font=('Arial',10,'bold')).pack(side='left', padx=(20,5))
        
        tk.Button(btn_control_frame, text='↑ High to Low', bg='#17A2B8', fg='white', bd=0, width=12,
                 font=('Arial',10,'bold'), command=lambda: self.sort_history('ic_desc')).pack(side='left', padx=2)
        
        tk.Button(btn_control_frame, text='↓ Low to High', bg='#17A2B8', fg='white', bd=0, width=12,
                 font=('Arial',10,'bold'), command=lambda: self.sort_history('ic_asc')).pack(side='left', padx=2)
        
        # Treeview Frame (with same styling as modify dialog)
        tree_frame = tk.Frame(win, bg=BG_COLOR, bd=2, relief='groove')
        tree_frame.grid(row=1, column=0, sticky='nsew', padx=10, pady=(5,10))
        tree_frame.grid_rowconfigure(0, weight=1)
        tree_frame.grid_columnconfigure(0, weight=1)
        
        # Updated columns for new history format
        cols = ('Time', 'Device Test', 'IC Name', 'Number of IC Test', 'Result Testing')
        self.history_tree = ttk.Treeview(tree_frame, columns=cols, show='headings')
        
        # Set column widths
        self.history_tree.column('Time', width=140)
        self.history_tree.column('Device Test', width=130)
        self.history_tree.column('IC Name', width=100)
        self.history_tree.column('Number of IC Test', width=130)
        self.history_tree.column('Result Testing', width=700)
        
        # Set column headings with better styling
        for c in cols:
            self.history_tree.heading(c, text=c, anchor='w')
        
        # Add scrollbars
        v_scrollbar = ttk.Scrollbar(tree_frame, orient='vertical', command=self.history_tree.yview)
        h_scrollbar = ttk.Scrollbar(tree_frame, orient='horizontal', command=self.history_tree.xview)
        self.history_tree.configure(yscrollcommand=v_scrollbar.set, xscrollcommand=h_scrollbar.set)
        
        # Grid layout for tree and scrollbars
        self.history_tree.grid(row=0, column=0, sticky='nsew', padx=5, pady=5)
        v_scrollbar.grid(row=0, column=1, sticky='ns', pady=5)
        h_scrollbar.grid(row=1, column=0, sticky='ew', padx=5)
        
        # Store original data for filtering and sorting
        self.original_history_data = []
        self.current_sort = 'time_desc'  # Default sort
        
        # Load initial data
        self.load_history_data()
        
        # Bind filter entries to real-time filtering
        self.ic_filter_var.trace('w', lambda *args: self.apply_history_filter())
        self.device_filter_var.trace('w', lambda *args: self.apply_history_filter())
    
    def load_history_data(self):
        """Load history data from database with thread-safe access"""
        self.original_history_data = []
        
        try:
            # Use thread-safe database access
            history_records = self.db_manager.get_all_history()
            
            if history_records:
                for row in history_records:
                    timestamp, device_id, program, parameters = row
                    
                    # Parse parameters: "ic_name;ic_test_count;result_testing"
                    param_parts = parameters.split(';', 2)  # Split into max 3 parts
                    if len(param_parts) >= 3:
                        ic_name = param_parts[0]
                        ic_test_count = param_parts[1]
                        result_testing = param_parts[2]
                    else:
                        # Fallback for old format or incomplete data
                        ic_name = param_parts[0] if len(param_parts) > 0 else "—"
                        ic_test_count = param_parts[1] if len(param_parts) > 1 else "—"
                        result_testing = param_parts[2] if len(param_parts) > 2 else "—"
                    
                    # Store as tuple for easy sorting and filtering
                    record = (timestamp, program, ic_name, ic_test_count, result_testing)
                    self.original_history_data.append(record)
                    
            print(f"Loaded {len(self.original_history_data)} history records from database")
            
        except Exception as e:
            print(f"Error loading history data: {e}")
        
        # Apply current sort and filter
        self.apply_history_filter()
    
    def apply_history_filter(self):
        """Apply current filters and display results"""
        # Get filter values
        ic_filter = self.ic_filter_var.get().strip().lower()
        device_filter = self.device_filter_var.get().strip().lower()
        
        # Filter data
        filtered_data = []
        for record in self.original_history_data:
            timestamp, program, ic_name, ic_test_count, result_testing = record
            
            # Apply IC name filter
            if ic_filter and ic_filter not in ic_name.lower():
                continue
                
            # Apply device test filter
            if device_filter and device_filter not in program.lower():
                continue
                
            filtered_data.append(record)
        
        # Apply current sort
        self.display_sorted_data(filtered_data)
    
    def clear_history_filter(self):
        """Clear all filters"""
        self.ic_filter_var.set('')
        self.device_filter_var.set('')
        self.apply_history_filter()
    
    def sort_history(self, sort_type):
        """Sort history data by different criteria"""
        self.current_sort = sort_type
        self.apply_history_filter()  # This will trigger sorting
    
    def display_sorted_data(self, data):
        """FIXED: Sort and display the data in treeview with proper date parsing"""
        # Sort data based on current sort type
        if self.current_sort == 'time_desc':
            # FIXED: Parse timestamp using our custom function
            def get_datetime(record):
                timestamp = record[0]  # "17/6/25 3:20:13"
                try:
                    # Split into date and time parts
                    parts = timestamp.split(' ')
                    if len(parts) >= 2:
                        date_part = parts[0]
                        time_part = parts[1]
                        return self.parse_date_time(date_part, time_part)
                    else:
                        return datetime.datetime.now()
                except:
                    return datetime.datetime.now()
            
            sorted_data = sorted(data, key=get_datetime, reverse=True)
            
        elif self.current_sort == 'time_asc':
            def get_datetime(record):
                timestamp = record[0]
                try:
                    parts = timestamp.split(' ')
                    if len(parts) >= 2:
                        date_part = parts[0]
                        time_part = parts[1]
                        return self.parse_date_time(date_part, time_part)
                    else:
                        return datetime.datetime.now()
                except:
                    return datetime.datetime.now()
            
            sorted_data = sorted(data, key=get_datetime)
            
        elif self.current_sort == 'ic_desc':
            # Sort by IC test count (numeric)
            try:
                sorted_data = sorted(data, key=lambda x: int(x[3]) if x[3].isdigit() else 0, reverse=True)
            except:
                sorted_data = sorted(data, key=lambda x: x[3], reverse=True)
        elif self.current_sort == 'ic_asc':
            try:
                sorted_data = sorted(data, key=lambda x: int(x[3]) if x[3].isdigit() else 0)
            except:
                sorted_data = sorted(data, key=lambda x: x[3])
        else:
            sorted_data = data
        
        # Clear existing items
        for item in self.history_tree.get_children():
            self.history_tree.delete(item)
        
        # Insert sorted data
        for record in sorted_data:
            self.history_tree.insert('', 'end', values=record)

if __name__ == '__main__':
    App().mainloop()