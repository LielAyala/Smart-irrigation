// routes/esp.js
const express = require('express');
const router = express.Router();
const fs = require('fs');
const path = require('path');
const tree = require('../modols/treeMode'); // וודא שהמודול Tree נמצא במיקום הנכון
const db = require('../modols/dataBase'); // ייבוא מסד הנתונים

// נתיב לקובץ ה-JSON
const jsonFilePath = path.join(__dirname, 'inside_information.json');

// מתחילים לבנות ניתוב
router.get('/', (req, res) => {
    const { temp, light, moisture } = req.query;

    console.log(req.query);
    console.log(light);
    console.log(moisture);

    res.status(200).json({ message: "Data received" }); // הגבה עם הודעה
});

// תחלופה לסוקט כי אין זמן לעשות את זה
router.get('/state', (req, res) => {
    let data = JSON.parse(fs.readFileSync('inside_information.json', 'utf8'));
    data.state=JSON.parse('SOIL_MOISTURE_MODE');
    res.send(data.state);
})

// ניתוב נוסף לשלוף נתונים מ-JSON על פי מצב
router.get('/dataMode', (req, res) => {
    const { state } = req.query; // לוקח את המצב 
    let data = JSON.parse(fs.readFileSync("inside_information.json", "utf8"));
    res.json(data[state]); // שליחה של המידע ל-ESP
});

module.exports = router;


// // routes/esp.js
// const express = require('express');
// const router = express.Router();
// const fs = require('fs');
// const tree = require('../modols/treeMode'); // וודא שהמודול Tree נמצא במיקום הנכון
// // const express = require('express');
// // const router = express.Router();
// // const fs = require('fs');
// // const db = require('../modols/dataBase'); 

// // מתחילים לבנות ניתוב
// router.get('/', (req, res) => {
//     const { temp, light, moisture } = req.query;

//     console.log(req.query);
//     console.log(light);
//     console.log(moisture);

//     res.status(200).json({ message: "Data received" }); // הגבה עם הודעה
// });

// // תחלופה לסוקט כי אין זמן לעשות את זה
// router.get('/state', (req, res) => {
//     let data = JSON.parse(fs.readFileSync("inside_information.json", "utf8"));
//     data = {
//         state: data.state,
//         data: new Data()
//     };
//     res.json(data); // גישה ל-key באובייקט 
// });

// // // קבלת מצב מהשרת
// // router.get('/state', (req, res) => {
// //     try {
// //         let data = JSON.parse(fs.readFileSync("inside_information.json", "utf8"));
// //         res.json({ state: data.state });
// //     } catch (error) {
// //         console.error("Error reading state:", error);
// //         res.status(500).json({ message: "Failed to retrieve state" });
// //     }
// // });

// //של גל 


// // קבלת נתוני מצב ספציפיים
// router.get('/dataMode', (req, res) => {
//     try {
//         const { state } = req.query;
//         let data = JSON.parse(fs.readFileSync("inside_information.json", "utf8"));
//         res.json(data[state] || {});
//     } catch (error) {
//         console.error("Error fetching mode data:", error);
//         res.status(500).json({ message: "Failed to retrieve mode data" });
//     }
// });

// // נתיב שמקבל את הנתונים מ-ESP
// router.get("/esp", async (req, res) => {
//     const { temp, light, moisture } = req.query;
//     console.log(`Temp: ${temp}, Light: ${light}, Moisture: ${moisture}`);
    
//     try {
//         // שליחה למסד הנתונים
//         await tree.storeESPData(temp, light, moisture);  
//         res.status(200).json({ message: "ESP data stored successfully" });
//     } catch (error) {
//         console.log(error);
//         res.status(500).json({ message: "Error storing ESP data" });
//     }
// });
// //ה-ESP יקרא לנתיב זה כל 10 דקות כדי לקבל את המצב הנוכחי.
// router.get('/checkStatus', async (req, res) => {
//     try {
//         let data = JSON.parse(fs.readFileSync("inside_information.json", "utf8"));
//         res.json({ status: data.state });
//     } catch (error) {
//         console.error("Error reading state:", error);
//         res.status(500).json({ error: "Failed to retrieve state" });
//     }
// });
// // ה-ESP ישלח לנתיב זה נתונים כל 3 שעות.
// router.post('/sendData', async (req, res) => {
//     try {
//         const { sensorName, plantNumber, value } = req.body;
        
//         if (!sensorName || !plantNumber || !value) {
//             return res.status(400).json({ error: "Missing parameters" });
//         }

//         // שמירת הנתונים במסד הנתונים
//         const query = `INSERT INTO sensor_data (sensor_name, plant_number, value, timestamp) VALUES (?, ?, ?, NOW())`;
//         await db.execute(query, [sensorName, plantNumber, value]);

//         res.json({ message: "Data received successfully" });
//     } catch (error) {
//         console.error("Error storing sensor data:", error);
//         res.status(500).json({ error: "Failed to store data" });
//     }
// });


// let pumpState = false; // משתנה לזכירת מצב המשאבה

// // קבלת נתוני החיישנים
// router.get('/getSensors', (req, res) => {
//     try {
//         let data = JSON.parse(fs.readFileSync("inside_information.json", "utf8"));
//         res.json({
//             temp: data.TEMP_MODE.temp || 0,
//             moisture: data.SOIL_MOISTURE_MODE.moisture || 0,
//             light: data.LIGHT_MODE.light || 0,
//             pump: false  // ניתן לשנות לפי הצורך
//         });
//     } catch (error) {
//         console.error("❌ שגיאה בקבלת נתוני החיישנים:", error);
//         res.status(500).json({ error: "שגיאה בשליפת נתונים מהשרת" });
//     }
// });


// // הפעלת וכיבוי המשאבה
// router.post('/togglePump', (req, res) => {
//     pumpState = !pumpState;
//     console.log(`🚰 מצב המשאבה שונה ל: ${pumpState ? "פועל" : "כבוי"}`);
//     res.json({ pump: pumpState });
// });







// module.exports = router;



