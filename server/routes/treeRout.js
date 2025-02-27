const express = require('express');
const router = express.Router();
const Tree = require('../modols/treeMode');
const db = require('../modols/dataBase');

const tree = new Tree(db);

router.post("/add", async (req, res) => {
    try {
        const { name } = req.body;
        if (!name) {
            return res.status(400).json({ message: "Missing tree name" });
        }

        await tree.createTree(name);
        res.status(201).json({ message: "Tree created successfully" });
    } catch (error) {
        console.error("Error creating tree:", error);
        res.status(500).json({ message: "Failed to create tree" });
    }
});



router.patch("/update/:id", async (req, res) => {
    const { name } = req.body;  // שם העץ החדש
    const { id } = req.params;   // מזהה העץ
    try {
        const result = await tree1.updateTree(id, name);
        res.status(200).json({ message: "Tree updated successfully" });
    } catch (error) {
        console.log(error);
        res.status(500).json({ message: "Error updating tree" });
    }
});

router.delete("/delete/:id", async (req, res) => {
    const { id } = req.params;
    try {
        await tree1.deleteTree(id);
        res.status(200).json({ message: "Tree deleted successfully" });
    } catch (error) {
        console.log(error);
        res.status(500).json({ message: "Error deleting tree" });
    }
});




module.exports=router;