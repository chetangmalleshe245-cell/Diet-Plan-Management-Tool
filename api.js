/**
 * api.js  –  Frontend ↔ C++ Backend bridge
 *
 * In a real deployment this points to the CGI endpoint.
 * For local demo (no server), DEMO_MODE = true uses localStorage simulation.
 */

const DEMO_MODE = true;   // ← set false when CGI is deployed
const CGI_URL   = "/cgi-bin/diet_api.cgi";

// ─── Demo in-memory store (replaces C++ file I/O for pure-frontend demo) ─
const DB = {
  users:   JSON.parse(localStorage.getItem("dp_users")  || "{}"),
  plans:   JSON.parse(localStorage.getItem("dp_plans")  || "{}"),
  otps:    {},
  save() {
    localStorage.setItem("dp_users", JSON.stringify(this.users));
    localStorage.setItem("dp_plans", JSON.stringify(this.plans));
  }
};

// ─── Utility ─────────────────────────────────────────────────
function uid() {
  return "UID_" + Date.now() + "_" + Math.floor(Math.random() * 9000 + 1000);
}
function planID(userID) {
  return "PLAN_" + userID.slice(0, 8) + "_" + Date.now();
}
function hashPwd(pwd) {
  // Simple demo hash — NOT cryptographic
  let h = 5381;
  for (let c of pwd + "DietSalt#2024")
    h = ((h << 5) + h) + c.charCodeAt(0);
  return (h >>> 0).toString(16).padStart(16, "0");
}
function bmi(weight, height) {
  const hm = height / 100;
  return weight / (hm * hm);
}
function bmiCat(b) {
  if (b < 18.5) return "Underweight";
  if (b < 25)   return "Normal weight";
  if (b < 30)   return "Overweight";
  return "Obese";
}
function tdee(weight, height, age, goal) {
  const bmr  = 10 * weight + 6.25 * height - 5 * age + 5;
  const base = bmr * 1.55;
  if (goal === "loss")    return Math.round(base - 500);
  if (goal === "gain")    return Math.round(base + 500);
  return Math.round(base);
}

// ─── Diet plan builder ───────────────────────────────────────
function buildMeals(goal, targetCal) {
  const plans = {
    loss: [
      { type:"Breakfast",         foods:"Oats with skimmed milk, 2 boiled eggs, black coffee",              pct:0.25, p:25, c:40, f:8  },
      { type:"Mid-Morning Snack", foods:"1 apple, 10 almonds",                                               pct:0.10, p:5,  c:20, f:8  },
      { type:"Lunch",             foods:"Grilled chicken 150g, brown rice 1 cup, cucumber salad",            pct:0.35, p:40, c:55, f:10 },
      { type:"Evening Snack",     foods:"Low-fat Greek yogurt, 1 banana",                                    pct:0.10, p:12, c:25, f:3  },
      { type:"Dinner",            foods:"Paneer tikka 100g / Fish 150g, steamed broccoli, 2 chapatis",      pct:0.20, p:30, c:30, f:12 },
    ],
    gain: [
      { type:"Breakfast",         foods:"4 whole eggs, 2 slices whole wheat toast, peanut butter, banana shake", pct:0.25, p:35, c:75, f:20 },
      { type:"Mid-Morning Snack", foods:"Whole milk 400ml, mixed nuts",                                           pct:0.15, p:20, c:60, f:15 },
      { type:"Lunch",             foods:"Chicken curry 200g, 2 cups white rice, dal, salad",                     pct:0.30, p:50, c:90, f:18 },
      { type:"Pre-Workout",       foods:"Sweet potato 150g, 2 boiled eggs",                                      pct:0.10, p:15, c:35, f:6  },
      { type:"Dinner",            foods:"Salmon 200g, pasta 1 cup, veggies, olive oil drizzle",                  pct:0.20, p:45, c:65, f:20 },
    ],
    maintain: [
      { type:"Breakfast",         foods:"2 eggs, 1 cup oats, fresh fruit, green tea",             pct:0.25, p:20, c:50, f:10 },
      { type:"Mid-Morning Snack", foods:"Fruit salad, handful of walnuts",                        pct:0.10, p:5,  c:25, f:8  },
      { type:"Lunch",             foods:"Dal rice / chapati sabzi, curd, salad",                  pct:0.35, p:25, c:70, f:10 },
      { type:"Evening Snack",     foods:"Sprouts chaat / roasted makhana, coconut water",         pct:0.10, p:8,  c:20, f:3  },
      { type:"Dinner",            foods:"Vegetable khichdi / grilled fish, 1 chapati, soup",      pct:0.20, p:20, c:45, f:8  },
    ]
  };
  return (plans[goal] || plans.maintain).map(m => ({
    ...m,
    calories: Math.round(targetCal * m.pct)
  }));
}

const NOTES = {
  loss:     "High protein, caloric deficit. Drink 3-4L water daily. Avoid sugar & processed food.",
  gain:     "Caloric surplus with high protein. Focus on compound lifts. Sleep 8+ hours.",
  maintain: "Balanced macros for sustainable health. Stay hydrated. Limit processed food."
};

// ─── Core API object ─────────────────────────────────────────
const API = {

  async _cgi(params) {
    const qs = new URLSearchParams(params).toString();
    const res = await fetch(`${CGI_URL}?${qs}`);
    return res.json();
  },

  // ── Request OTP ───────────────────────────────────────────
  async requestOTP(email) {
    if (!DEMO_MODE) return this._cgi({ action: "request_otp", email });

    if (DB.users[email]) return { status: "error", message: "Email already registered." };
    const otp = String(Math.floor(100000 + Math.random() * 900000));
    DB.otps[email] = { otp, ts: Date.now() };
    console.log(`%c[OTP for ${email}]: ${otp}`, "color:orange;font-weight:bold");
    return { status: "ok", otp, email };   // in prod, otp would NOT be returned to frontend
  },

  // ── Login OTP ────────────────────────────────────────────
  async loginOTP(email, password) {
    if (!DEMO_MODE) return this._cgi({ action: "login", email, password });

    const user = DB.users[email];
    if (!user)                        return { status: "error", message: "Email not found." };
    if (user.passwordHash !== hashPwd(password))
                                      return { status: "error", message: "Incorrect password." };
    const otp = String(Math.floor(100000 + Math.random() * 900000));
    DB.otps[email] = { otp, ts: Date.now() };
    console.log(`%c[Login OTP for ${email}]: ${otp}`, "color:lime;font-weight:bold");
    return { status: "ok", otp, email };
  },

  // ── Register ─────────────────────────────────────────────
  async register({ name, age, weight, height, goal, email, password, otp }) {
    if (!DEMO_MODE) return this._cgi({ action:"register", name, age, weight, height, goal, email, password, otp });

    const stored = DB.otps[email];
    if (!stored || stored.otp !== otp || Date.now() - stored.ts > 300000)
      return { status: "error", message: "Invalid or expired OTP." };
    if (DB.users[email])
      return { status: "error", message: "Email already registered." };
    if (password.length < 6)
      return { status: "error", message: "Password too short (min 6 chars)." };

    const userID = uid();
    const user = { userID, name, age: +age, weight: +weight, height: +height,
                   goal, email, passwordHash: hashPwd(password),
                   registrationDate: new Date().toISOString().slice(0,10),
                   totalPlansGenerated: 0, loggedCalories: 0 };
    DB.users[email] = user;
    delete DB.otps[email];
    DB.save();
    return { status:"ok", userID, name, email, goal };
  },

  // ── Verify login OTP ─────────────────────────────────────
  async verifyLogin(email, otp) {
    if (!DEMO_MODE) return this._cgi({ action:"verify_login", email, otp });

    const stored = DB.otps[email];
    if (!stored || stored.otp !== otp || Date.now() - stored.ts > 300000)
      return { status:"error", message:"Invalid or expired OTP." };
    delete DB.otps[email];

    const user = DB.users[email];
    if (!user) return { status:"error", message:"User not found." };

    const b = bmi(user.weight, user.height);
    return { status:"ok", userID: user.userID, name: user.name,
             email: user.email, goal: user.goal,
             bmi: b.toFixed(1), bmiCategory: bmiCat(b),
             targetCalories: tdee(user.weight, user.height, user.age, user.goal) };
  },

  // ── Generate Plan ────────────────────────────────────────
  async generatePlan(userID) {
    if (!DEMO_MODE) return this._cgi({ action:"generate_plan", userID });

    const user = Object.values(DB.users).find(u => u.userID === userID);
    if (!user) return { status:"error", message:"User not found." };

    const tCal  = tdee(user.weight, user.height, user.age, user.goal);
    const meals = buildMeals(user.goal, tCal);
    const pid   = planID(userID);
    const plan  = { planID: pid, userID, goal: user.goal,
                    date: new Date().toISOString().slice(0,10),
                    targetCalories: tCal,
                    totalCalories:  meals.reduce((a,m)=>a+m.calories,0),
                    totalProtein:   meals.reduce((a,m)=>a+m.p,0),
                    totalCarbs:     meals.reduce((a,m)=>a+m.c,0),
                    totalFats:      meals.reduce((a,m)=>a+m.f,0),
                    notes:          NOTES[user.goal],
                    meals };
    if (!DB.plans[userID]) DB.plans[userID] = [];
    DB.plans[userID].push(plan);
    user.totalPlansGenerated++;
    DB.save();
    return { status:"ok", ...plan };
  },

  // ── Get History ──────────────────────────────────────────
  async getHistory(userID) {
    if (!DEMO_MODE) return this._cgi({ action:"get_history", userID });
    const plans = DB.plans[userID] || [];
    return { status:"ok", plans };
  },

  // ── Get Progress ─────────────────────────────────────────
  async getProgress(userID) {
    if (!DEMO_MODE) return this._cgi({ action:"get_progress", userID });
    const user = Object.values(DB.users).find(u => u.userID === userID);
    if (!user) return { status:"error", message:"User not found." };
    const b    = bmi(user.weight, user.height);
    const tCal = tdee(user.weight, user.height, user.age, user.goal);
    return { status:"ok", userID, name: user.name, goal: user.goal,
             bmi: +b.toFixed(1), bmiCategory: bmiCat(b),
             targetCalories: tCal, loggedCalories: user.loggedCalories || 0,
             remainingCalories: tCal - (user.loggedCalories || 0),
             weight: user.weight, height: user.height,
             totalPlans: user.totalPlansGenerated };
  },

  // ── Log Calories ─────────────────────────────────────────
  async logCalories(userID, calories) {
    if (!DEMO_MODE) return this._cgi({ action:"log_calories", userID, calories });
    const user = Object.values(DB.users).find(u => u.userID === userID);
    if (!user) return { status:"error", message:"User not found." };
    user.loggedCalories = (user.loggedCalories || 0) + +calories;
    DB.users[user.email] = user;
    DB.save();
    return { status:"ok", logged: calories };
  },

  // ── Update Profile ───────────────────────────────────────
  async updateProfile(userID, { weight, height, goal }) {
    if (!DEMO_MODE) return this._cgi({ action:"update_profile", userID, weight, height, goal });
    const user = Object.values(DB.users).find(u => u.userID === userID);
    if (!user) return { status:"error", message:"User not found." };
    user.weight = +weight; user.height = +height; user.goal = goal;
    DB.users[user.email] = user;
    DB.save();
    return { status:"ok", userID };
  }
};
