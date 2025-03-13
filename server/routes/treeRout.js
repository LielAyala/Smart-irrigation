
const express = require('express');
const router = express.Router();
const Tree = require('../modols/treeMode');
const db = require('../modols/dataBase');

const tree = new Tree(db);

//  יצירת צמח חדש
router.post("/plant/add", async (req, res) => {
    try {
        const { name } = req.body;
        if (!name) {
            return res.status(400).json({ message: "Missing plant name" });
        }

        await tree.createPlant(name);
        res.status(201).json({ message: "Plant created successfully" });
    } catch (error) {
        console.error("Error creating plant:", error);
        res.status(500).json({ message: "Failed to create plant" });
    }
});

//  יצירת עץ חדש
router.post("/tree/add", async (req, res) => {
    try {
        const { idPlant } = req.body;
        if (!idPlant) {
            return res.status(400).json({ message: "Missing plant ID" });
        }

        await tree.createTree(idPlant);
        res.status(201).json({ message: "Tree created successfully" });
    } catch (error) {
        console.error("Error creating tree:", error);
        res.status(500).json({ message: "Failed to create tree" });
    }
});

// עדכון שם צמח לפי שם (ולא ID)
router.patch("/plant/update", async (req, res) => {
    try {
        const { oldName, newName } = req.body;

        if (!oldName || !newName) {
            return res.status(400).json({ message: "Missing plant name(s)" });
        }

        await db.execute("UPDATE Plants SET name = ? WHERE name = ?", [newName, oldName]);
        res.status(200).json({ message: "Plant updated successfully" });
    } catch (error) {
        console.error("Error updating plant:", error);
        res.status(500).json({ message: "Error updating plant" });
    }
});

//  מחיקת עץ לפי שם (ולא ID)
router.delete("/tree/delete", async (req, res) => {
    try {
        const { name } = req.body;

        if (!name) {
            return res.status(400).json({ message: "Missing tree name" });
        }

        await db.execute("DELETE FROM Threes WHERE id_plants = (SELECT id FROM Plants WHERE name = ?)", [name]);
        res.status(200).json({ message: "Tree deleted successfully" });
    } catch (error) {
        console.error("Error deleting tree:", error);
        res.status(500).json({ message: "Error deleting tree" });
    }
});

//  מחיקת צמח לפי שם
router.delete("/plant/delete", async (req, res) => {
    try {
        const { name } = req.body;

        if (!name) {
            return res.status(400).json({ message: "Missing plant name" });
        }

        await db.execute("DELETE FROM Plants WHERE name = ?", [name]);
        res.status(200).json({ message: "Plant deleted successfully" });
    } catch (error) {
        console.error("Error deleting plant:", error);
        res.status(500).json({ message: "Error deleting plant" });
    }
});
//  שליפת כל הצמחים
router.get("/plant/all", async (req, res) => {
    try {
        let [plants] = await db.execute("SELECT * FROM Plants");
        res.status(200).json(plants);
    } catch (error) {
        console.error("Error fetching plants:", error);
        res.status(500).json({ message: "Failed to fetch plants" });
    }
});


//  הצגת כל העצים עם שמות הצמחים
router.get("/tree/all", async (req, res) => {
    try {
        const trees = await tree.getAllTrees();
        res.status(200).json(trees);
    } catch (error) {
        console.error("Error fetching trees:", error);
        res.status(500).json({ message: "Failed to fetch trees" });
    }
});

//  הוספת מידע מחיישן לעץ
router.post("/sensor/add", async (req, res) => {
    try {
        const { idTree, nameSensor, avg, isRunning } = req.body;
        if (!idTree || !nameSensor || avg === undefined || isRunning === undefined) {
            return res.status(400).json({ message: "Missing sensor data" });
        }

        await tree.addSensorData(idTree, nameSensor, avg, isRunning);
        res.status(201).json({ message: "Sensor data added successfully" });
    } catch (error) {
        console.error("Error adding sensor data:", error);
        res.status(500).json({ message: "Failed to add sensor data" });
    }
});

//  הצגת כל הנתונים מחיישנים של עץ מסוים
router.get("/sensor/:idTree", async (req, res) => {
    try {
        const { idTree } = req.params;
        const data = await tree.getSensorData(idTree);
        res.status(200).json(data);
    } catch (error) {
        console.error("Error fetching sensor data:", error);
        res.status(500).json({ message: "Failed to fetch sensor data" });
    }
});

module.exports = router;

