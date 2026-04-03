# 🥗 NutriPlan – Diet Plan Management System

> A full-stack Diet Plan Management System built with **C++ (OOP)** backend, **HTML/CSS/JS** frontend, and **file-based storage**. Designed for 2nd Year BE CSE students.

---

## 📐 Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                        FRONTEND (Browser)                       │
│  ┌─────────────┐  ┌──────────────┐  ┌────────────────────────┐ │
│  │  index.html  │  │  styles.css  │  │  app.js  │  api.js     │ │
│  │  (SPA shell) │  │  (UI/layout) │  │  (logic) │  (bridge)   │ │
│  └──────┬──────┘  └──────────────┘  └─────┬────┴────────────┘ │
│         │                                  │  HTTP / CGI call   │
└─────────┼──────────────────────────────────┼────────────────────┘
          │                                  │
          ▼                                  ▼
┌─────────────────────────────────────────────────────────────────┐
│                     BACKEND (C++ CGI)                           │
│                                                                 │
│  main.cpp  ←──  routes action= query param                      │
│     │                                                           │
│     ├──▶  AuthManager  ──▶  OTPService                          │
│     │         │                  │                              │
│     ├──▶  DietManager            │                              │
│     │                            ▼                              │
│     └──▶  FileManager  (all I/O centralised here)              │
│                                                                 │
│  OOP Hierarchy:                                                 │
│    User (abstract base)                                         │
│      └── RegisteredUser (concrete, serialisable)                │
│    DietPlan (composition of Meals)                              │
│    AuthManager / DietManager / FileManager / OTPService         │
└─────────────────────────────────────────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────────────┐
│                    FILE STORAGE  (/data)                         │
│                                                                 │
│  users.txt              ← all registered users (one per line)   │
│  otps.txt               ← pending OTPs with timestamps          │
│  users/<uid>_plans.txt  ← diet plan history per user            │
└─────────────────────────────────────────────────────────────────┘
```

---

## 📁 Folder Structure

```
diet-plan-system/
├── frontend/
│   ├── index.html          ← Single-page app (all views)
│   ├── css/
│   │   └── styles.css      ← Full responsive stylesheet
│   └── js/
│       ├── api.js          ← Backend API bridge (CGI or demo mode)
│       └── app.js          ← UI logic, routing, event handlers
│
├── backend/
│   ├── Makefile            ← Build system
│   ├── include/            ← Header files (.h)
│   │   ├── User.h          ← Abstract base class
│   │   ├── RegisteredUser.h← Derived class
│   │   ├── DietPlan.h      ← Diet plan + Meal structs
│   │   ├── FileManager.h   ← File I/O abstraction
│   │   ├── AuthManager.h   ← Registration & login logic
│   │   ├── OTPService.h    ← OTP generation & verification
│   │   └── DietManager.h   ← Plan generation & history
│   └── src/                ← Implementation files (.cpp)
│       ├── main.cpp        ← CGI entry point / router
│       ├── User.cpp
│       ├── RegisteredUser.cpp
│       ├── DietPlan.cpp
│       ├── FileManager.cpp
│       ├── AuthManager.cpp
│       ├── OTPService.cpp
│       └── DietManager.cpp
│
├── data/
│   ├── users.txt           ← User records
│   ├── otps.txt            ← Pending OTPs (auto-generated)
│   └── users/              ← Per-user plan files
│       └── <uid>_plans.txt
│
└── README.md
```

---

## 🔷 OOP Concepts Used

| Concept | Where Used |
|---|---|
| **Abstraction** | `User` is abstract with pure virtual `displayInfo()` and `getRole()` |
| **Encapsulation** | All member data is `private`/`protected`; access via getters/setters |
| **Inheritance** | `RegisteredUser` inherits from `User` |
| **Polymorphism** | `displayInfo()` overridden; `User*` pointer can hold `RegisteredUser` |
| **Classes & Objects** | `User`, `RegisteredUser`, `DietPlan`, `Meal`, `FileManager`, `AuthManager`, `OTPService`, `DietManager` |

---

## 🔌 Frontend ↔ Backend Integration

### Option A: CGI (Deployed)
```
Apache/Nginx  →  /cgi-bin/diet_api.cgi  ←  Query string params
Frontend JS fetches: GET /cgi-bin/diet_api.cgi?action=login&email=...
```

### Option B: Demo Mode (Default, no server needed)
`api.js` has `DEMO_MODE = true` — all data is stored in **localStorage**.
This lets you run the frontend by simply opening `index.html` in a browser.

### Query String API Reference
| Action | Params | Description |
|---|---|---|
| `request_otp` | `email` | Send OTP for registration |
| `register` | `name,age,weight,height,goal,email,password,otp` | Create account |
| `login` | `email,password` | Initiate login, sends OTP |
| `verify_login` | `email,otp` | Complete login, returns user |
| `generate_plan` | `userID` | Generate + save new diet plan |
| `get_history` | `userID` | Fetch all past plans |
| `get_progress` | `userID` | BMI, calories, stats |
| `log_calories` | `userID,calories` | Log eaten calories |
| `update_profile` | `userID,weight,height,goal` | Update profile |

---

## 🚀 How to Run Locally

### Frontend Only (Demo Mode – recommended for learning)
```bash
# Just open in browser — no server needed
open frontend/index.html        # macOS
xdg-open frontend/index.html    # Linux
start frontend/index.html       # Windows
```
All data is stored in browser `localStorage`. OTPs are auto-filled and logged to browser console.

### Full Stack (with C++ CGI backend)

#### Step 1: Install Apache + CGI support
```bash
# Ubuntu / Debian
sudo apt install apache2 libapache2-mod-cgi
sudo a2enmod cgi
sudo systemctl restart apache2
```

#### Step 2: Compile C++ backend
```bash
cd backend
make
# Produces: diet_api.cgi
```

#### Step 3: Deploy CGI
```bash
sudo cp diet_api.cgi /usr/lib/cgi-bin/
sudo chmod +x /usr/lib/cgi-bin/diet_api.cgi

# Create data directory accessible by Apache
sudo mkdir -p /var/www/data/users
sudo chown -R www-data:www-data /var/www/data
```

#### Step 4: Update paths
In `backend/src/main.cpp`, change the FileManager path:
```cpp
FileManager fm("/var/www/data");   // absolute path
```
In `frontend/js/api.js`, set:
```js
const DEMO_MODE = false;
const CGI_URL   = "/cgi-bin/diet_api.cgi";
```

#### Step 5: Deploy frontend
```bash
sudo cp -r frontend/* /var/www/html/
```

#### Step 6: Open browser
```
http://localhost/index.html
```

---

## 📤 Push to GitHub

```bash
# 1. Create repo on github.com (name: diet-plan-system)

# 2. Initialize locally
git init
git add .
git commit -m "Initial commit: Diet Plan Management System"

# 3. Connect and push
git remote add origin https://github.com/YOUR_USERNAME/diet-plan-system.git
git branch -M main
git push -u origin main
```

### Recommended `.gitignore`
```
backend/diet_api.cgi
data/otps.txt
data/users.txt
data/users/
*.o
*.out
```

---

## 🧪 Testing the Backend Standalone

```bash
cd backend
make
# Test request OTP
QUERY_STRING="action=request_otp&email=test@test.com" ./diet_api.cgi

# Test register (after getting OTP from above)
QUERY_STRING="action=register&name=Test+User&age=21&weight=70&height=175\
&goal=loss&email=test@test.com&password=pass123&otp=XXXXXX" ./diet_api.cgi
```

---

## 🍎 Features Summary

| Feature | Status |
|---|---|
| User Registration (name, age, weight, height, goal) | ✅ |
| OTP verification (simulated) | ✅ |
| Secure password hashing | ✅ |
| Login with OTP 2FA | ✅ |
| BMI calculation (Mifflin-St Jeor) | ✅ |
| Diet plan generation (loss/gain/maintain) | ✅ |
| Macro breakdown (protein/carbs/fats) | ✅ |
| Daily calorie logging & progress bar | ✅ |
| Diet plan history per user | ✅ |
| Profile update (weight/height/goal) | ✅ |
| File-based storage (no database) | ✅ |
| Responsive UI (mobile friendly) | ✅ |
| All 5 OOP concepts demonstrated | ✅ |

---

## 👨‍🎓 For Students: Key Learning Points

1. **Why abstract classes?** `User` can't be instantiated directly — forces correct subclassing.
2. **Why FileManager as a separate class?** Single Responsibility Principle — all I/O in one place makes it easy to swap to a DB later.
3. **Why OTPService takes FileManager by reference?** Dependency injection — loose coupling.
4. **CGI vs REST:** CGI is the simplest way to expose C++ logic as a web API. Production systems would use a proper REST server (like Crow or Drogon for C++).
5. **Demo mode in api.js:** Lets you test the full UI without compiling C++ — great for frontend-first development.

---

*Built with ❤️ as a 2nd Year BE CSE Project*
