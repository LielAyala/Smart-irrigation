// routes/esp.js
const express = require('express');
const router = express.Router();
const fs = require('fs');
const path = require('path');
const tree = require('../modols/treeMode'); // וודא שהמודול Tree נמצא במיקום הנכון
const db = require('../modols/dataBase'); // ייבוא מסד הנתונים


// מיפוי מספרים לשמות המצבים
const stateMapping = {
    "61": "TEMP_MODE",
    "62": "SOIL_MOISTURE_MODE",
    "63": "SATURDAY_MODE",
    "64": "MANUAL_MODE"
};


// נתיב לקובץ ה-JSON
const jsonFilePath = path.join(__dirname, '../inside_information.json');
// מתחילים לבנות ניתוב
router.get('/', (req, res) => {
    const { temp, light, moisture } = req.query;
    console.log(req.query);
    //console.log(light);
    //console.log(moisture);
    res.status(200).json({ message: "Data received" }); // הגבה עם הודעה
});


router.get('/state', (req, res) => {
    try {
        // קריאה של הנתונים מהקובץ
        let data = JSON.parse(fs.readFileSync(jsonFilePath, 'utf8'));
        // שליחת הנתונים חזרה ללקוח
        res.json({
            message: "State data retrieved successfully",
            CurrentStatus: data.state,
        });
        console.log("State data sent successfully");
    } catch (error) {
        console.error("Error reading state file:", error);
        res.status(500).json({ error: "Error retrieving state data" });
    }
});
router.get('/state=:state', (req, res) => {
    try {

        let data = JSON.parse(fs.readFileSync(jsonFilePath, 'utf8'));// קריאת הנתונים מהקובץ
        let newState = req.params.state; // עכשיו אנחנו לוקחים את ה-state מה-path
        data.state = newState;// עדכון ה-state בקובץ
        fs.writeFileSync(jsonFilePath, JSON.stringify(data, null, 2), 'utf8');//כתיבת ה state ל inside_information
        console.log("State updated to: ${newState}");
        // החזרת ה-state החדש
        res.json({ message: "State updated", CurrentStatus: data.state });

    } catch (error) {
        console.error("Error updating state:", error);
        res.status(500).json({ error: "Error updating state" });
    }
});
// שליחת הנתונים לפי מצב מבוקש
router.get('/dataMode', (req, res) => {
    try {
        const { state } = req.query; // מקבל את המצב המבוקש מהבקשה
        if (!state) {
            return res.status(400).json({ error: "State parameter is required" });
        }

        let data = JSON.parse(fs.readFileSync(jsonFilePath, 'utf8'));

        if (!data[state]) {
            return res.status(404).json({ error: `State '${state}' not found in JSON` });
        }

        res.json(data[state]); // מחזיר את הנתונים של המצב המבוקש
    } catch (error) {
        console.error(" שגיאה בקריאת המידע:", error);
        res.status(500).json({ error: "Failed to retrieve data" });
    }
});
// נתיב לקליטת דגימות חיישנים שנשלחות מהארדואינו
// router.post('/sendSample', async (req, res) => {
//     try {
//         // שליפת הנתונים מהבקשה שהתקבלה
//         const { sensorName, measurementValue } = req.body;

//         // הדפסת הנתונים שהתקבלו למסוף השרת לצורך מעקב
//         console.log("נתונים שהתקבלו מהארדואינו:", { sensorName, measurementValue });

//         // בדיקה אם חסרים נתונים קריטיים בבקשה
//         if (!sensorName || measurementValue === undefined) {
//             return res.status(400).json({ message: 'נתונים חסרים, בדקו את השליחה מהארדואינו!' });
//         }

//         // בדיקה אם קיים עץ כלשהו במערכת
//         let [treeResult] = await db.execute(`SELECT id FROM threes ORDER BY id DESC LIMIT 1`);

//         let treeId;
//         // אם אין עץ במערכת, יוצרים עץ חדש
//         if (treeResult.length === 0) {
//             console.log("לא נמצא עץ קיים – יוצרים עץ חדש...");
            
//             // הכנסת עץ חדש לטבלה `threes`
//             let [newTree] = await db.execute(`INSERT INTO threes (id_plants, date) VALUES (?, NOW())`, [1]);

//             // שמירת מזהה העץ החדש שנוסף
//             treeId = newTree.insertId;
//         } else {
//             // אם נמצא עץ קיים, משתמשים בו
//             treeId = treeResult[0].id;
//         }

//         // הדפסת השיוך של הדגימה לעץ שנבחר או שנוסף
//         console.log(`משייכים את הדגימה לעץ ${treeId}`);

//         // הכנסת הדגימה לטבלת `datasensors`
//         await db.execute(
            
//             `INSERT INTO datasensors (id_threes, name_sensor, avg, date, isRunning) 
//              VALUES (?, ?, ?, NOW(), ?)`,
//             [treeId, sensorName, measurementValue, 1] // כאן הוספנו ערך ברירת מחדל ל- isRunning
//         );
        

//         // החזרת תגובה שהדגימה נשמרה בהצלחה
//         res.status(201).json({ message: `הדגימה של ${sensorName} נשמרה תחת עץ ${treeId} בהצלחה!` });

//     } catch (error) {
//         // הדפסת שגיאה במקרה של כשל
//         console.error('שגיאה בשליחת הדגימה:', error);

//         // שליחת תגובה עם שגיאה ללקוח
//         res.status(500).json({ message: `שגיאה בשרת: ${error.sqlMessage || error.message}` });
//     }
// });


// נתיב לעדכון נתונים בקובץ JSON
// פונקציה להמרת ערכים למספרים אם אפשר
const convertValuesToNumbers = (obj) => {
    for (let key in obj) {
        if (!isNaN(obj[key]) && obj[key] !== "" && obj[key] !== null) {
            obj[key] = Number(obj[key]); // המרה למספר
        }
    }
    return obj;
};

// נתיב לעדכון inside_information.json
router.post('/updateInsideInformation', (req, res) => {
    try {
        console.log(" קיבלנו נתונים מהלקוח:", req.body);

        let data = JSON.parse(fs.readFileSync("inside_information.json", "utf8"));
        let { state, updatedData } = req.body;

        // המרת state ממספר לשם (אם קיים במיפוי)
        let mappedState = stateMapping[state] || state;

        if (!mappedState || !updatedData) {
            return res.status(400).json({ error: " חסר state או updatedData בגוף הבקשה" });
        }

        if (!data[mappedState]) {
            return res.status(400).json({ error: ` מצב ${mappedState} אינו קיים בקובץ JSON` });
        }

        // המרת מחרוזות למספרים לפני שמירה
        let convertedData = convertValuesToNumbers(updatedData);

        // עדכון ושמירה בקובץ JSON
        data[mappedState] = { ...data[mappedState], ...convertedData };
        fs.writeFileSync("inside_information.json", JSON.stringify(data, null, 2), "utf8");

        console.log(" עדכון הושלם בהצלחה!", data[mappedState]);
        res.json({ message: "Updated successfully", data: data[mappedState] });
    } catch (error) {
        console.error(" שגיאה בעדכון הקובץ:", error);
        res.status(500).json({ error: "Failed to update data" });
    }
});
//נתיב לקבלת נתוני צריכת מים מהארדואינו ושמירתם בטבלת 
router.post('/waterUsage', async (req, res) => {
    try {
        let { treeID, waterUsageToday } = req.body;  

        console.log("נתוני צריכת מים שהתקבלו:", { treeID, waterUsageToday });

        if (!treeID || waterUsageToday === undefined) {
            return res.status(400).json({ message: 'נתונים חסרים, בדקו את השליחה מהארדואינו!' });
        }

        // בדיקה אם האי די של העץ קיים 
        let [treeExists] = await db.execute(`SELECT id FROM threes WHERE id = ?`, [treeID]);

        if (treeExists.length === 0) {
            console.log(` העץ עם treeID=${treeID} לא נמצא!`);
            return res.status(404).json({ message: ` העץ עם treeID=${treeID} לא נמצא במערכת!` });
        }

        //  הכנסת נתוני המים לטבלת 
        await db.execute(`
            INSERT INTO datasensors (id_threes, name_sensor, avg, date, isRunning) 
            VALUES (?, ?, ?, NOW(), ?)`, 
            [treeID, "WaterUsage", waterUsageToday, 1]
        );

        console.log(` נתוני המים נשמרו בטבלה datasensors עבור עץ ${treeID}: ${waterUsageToday} מ"ל`);
        res.status(201).json({ message: `נתוני המים נשמרו בהצלחה עבור עץ ${treeID}!` });

    } catch (error) {
        console.error(' שגיאה בשליחת נתוני המים:', error);
        res.status(500).json({ message: `שגיאה בשרת: ${error.sqlMessage || error.message}` });
    }
});



// **נתיב לקבלת נתוני צריכת מים מתוך datasensors**
router.get('/waterUsage/:treeID', async (req, res) => {
    try {
        const { treeID } = req.params;
        if (!treeID) {
            return res.status(400).json({ message: 'חסר treeID בשאילתא' });
        }

        let [waterData] = await db.execute(`
            SELECT avg AS waterAmount, date FROM datasensors 
            WHERE id_threes = ? AND name_sensor = "WaterUsage" 
            ORDER BY date DESC LIMIT 1`, 
            [treeID]
        );

        if (waterData.length === 0) {
            return res.status(404).json({ message: 'לא נמצאו נתוני צריכת מים לעץ זה' });
        }

        res.json(waterData[0]);

    } catch (error) {
        console.error('שגיאה בשליפת נתוני המים:', error);
        res.status(500).json({ message: 'שגיאה בשרת' });
    }
});


module.exports = router;

