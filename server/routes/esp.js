// routes/esp.js
const express = require('express');
const router = express.Router();
const fs = require('fs');
const path = require('path');
const tree = require('../modols/treeMode'); // וודא שהמודול Tree נמצא במיקום הנכון
const db = require('../modols/dataBase'); // ייבוא מסד הנתונים

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
        console.error("❌ שגיאה בקריאת המידע:", error);
        res.status(500).json({ error: "Failed to retrieve data" });
    }
});
router.post('/sendSample', async (req, res) => {
    try {
        const { sensorName, measurementValue } = req.body;

        console.log("📌 נתונים שהתקבלו מהארדואינו:", { sensorName, measurementValue });

        if (!sensorName || measurementValue === undefined) {
            return res.status(400).json({ message: '❌ נתונים חסרים, בדקו את השליחה מהארדואינו!' });
        }

        // בדיקה אם החיישן קיים בטבלה `sensors`
        let [sensorResult] = await db.execute(`SELECT id FROM sensors WHERE name = ?`, [sensorName]);

        if (sensorResult.length === 0) {
            console.log(`🔍 חיישן ${sensorName} לא נמצא, מוסיף למסד הנתונים...`);
            let [insertResult] = await db.execute(`INSERT INTO sensors (name, isRunning) VALUES (?, 1)`, [sensorName]);

            if (!insertResult.insertId || insertResult.insertId === 0) {
                console.error("❌ שגיאה: הכנסת חיישן נכשלה, ID לא נוצר");
                return res.status(500).json({ message: "❌ שגיאה ביצירת חיישן חדש" });
            }

            sensorResult = [{ id: insertResult.insertId }];
        }

        const sensorId = sensorResult[0].id;

        // מציאת עץ מתאים או יצירת אחד חדש
        let treeId;
        let [treeResult] = await db.execute(`SELECT id FROM threes ORDER BY id DESC LIMIT 1`);

        if (treeResult.length === 0) {
            console.log("🌳 לא נמצא עץ קיים, יוצר עץ חדש...");
            let [newTree] = await db.execute(`INSERT INTO threes (id_plants, date) VALUES (?, NOW())`, [1]);
            treeId = newTree.insertId;
        } else {
            treeId = treeResult[0].id;
        }

        // בדיקה סופית שאין `treeId=0` או `sensorId=0`
        if (!treeId || treeId === 0 || !sensorId || sensorId === 0) {
            console.error(`❌ שגיאה: treeId=${treeId}, sensorId=${sensorId} - לא תקינים`);
            return res.status(500).json({ message: '❌ שגיאה: לא ניתן למצוא או ליצור עץ/חיישן מתאים!' });
        }

        console.log(`✅ מכניסים דגימה: treeId=${treeId}, sensorId=${sensorId}, avg=${measurementValue}`);

        // הכנסת הדגימה לטבלה `datasensors`
        await db.execute(
            `INSERT INTO datasensors (id_trees, id_sensors, avg, date) VALUES (?, ?, ?, NOW())`,
            [treeId, sensorId, measurementValue]
        );

        res.status(201).json({ message: `✅ הדגימה של ${sensorName} נשמרה תחת עץ ${treeId} בהצלחה!` });

    } catch (error) {
        console.error('❌ שגיאה בשליחת הדגימה:', error);
        res.status(500).json({ message: `❌ שגיאה בשרת: ${error.sqlMessage || error.message}` });
    }
});




router.get('/samples', async (req, res) => {
    try {
        const [samples] = await db.execute(`
            SELECT d.id, t.id AS treeId, s.name AS sensorName, d.avg, d.date 
            FROM datasensors d
            JOIN threes t ON d.id_trees = t.id
            JOIN sensors s ON d.id_sensors = s.id
            ORDER BY d.date DESC
        `);

        res.json(samples);
    } catch (error) {
        console.error('שגיאה בשליפת הדגימות:', error);
        res.status(500).json({ message: 'שגיאה בשרת' });
    }
});





module.exports = router;

