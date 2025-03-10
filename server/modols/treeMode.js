// treeModel.js
class Tree {
    constructor(db) {
        this.DB = db;
    }

    // ✅ יצירת צמח חדש
    async createPlant(name) {
        await this.DB.execute(`INSERT INTO Plants (name) VALUES (?)`, [name]);
    }

    // ✅ יצירת עץ חדש המקושר לצמח
    async createTree(idPlant) {
        await this.DB.execute(`INSERT INTO Threes (id_plants) VALUES (?)`, [idPlant]);
    }

    // ✅ עדכון שם צמח לפי ID
    async updatePlant(id, newName) {
        await this.DB.execute(`UPDATE Plants SET name = ? WHERE ID = ?`, [newName, id]);
    }

    // ✅ מחיקת עץ לפי ID (כולל זמני השקיה וחיישנים)
    async deleteTree(id) {
        await this.DB.execute(`DELETE FROM DataSensors WHERE id_threes = ?`, [id]); // מוחק מידע מחיישנים
        await this.DB.execute(`DELETE FROM Threes WHERE id = ?`, [id]); // מוחק את העץ
    }

    // ✅ שליפת כל העצים והצמחים המקושרים
    async getAllTrees() {
        let [rows] = await this.DB.execute(`
            SELECT t.id, p.name, t.date 
            FROM Threes t 
            JOIN Plants p ON t.id_plants = p.ID
        `);
        return rows;
    }

    // ✅ הוספת נתוני חיישן לעץ מסוים
    async addSensorData(idTree, nameSensor, avg, isRunning) {
        await this.DB.execute(`
            INSERT INTO DataSensors (id_threes, name_sensor, avg, isRunning) 
            VALUES (?, ?, ?, ?)
        `, [idTree, nameSensor, avg, isRunning]);
    }

    // ✅ שליפת כל נתוני החיישנים לעץ מסוים
    async getSensorData(idTree) {
        let [rows] = await this.DB.execute(`
            SELECT name_sensor, avg, date, isRunning 
            FROM DataSensors 
            WHERE id_threes = ?
        `, [idTree]);
        return rows;
    }

    // ✅ מחיקת נתוני חיישן
    async deleteSensorData(id) {
        await this.DB.execute(`DELETE FROM DataSensors WHERE id = ?`, [id]);
    }
}

module.exports = Tree;



// class Tree {
//     constructor(db) {
//         this.DB = db;
//     }

//     async createTree(nameTree) {
//         try {
//             // בדיקה אם הצמח קיים
//             let [rows] = await this.DB.execute(`SELECT id FROM plants WHERE name = ?`, [nameTree]);

//             let plantId;
//             if (rows.length > 0) {
//                 plantId = rows[0].id;
//             } else {
//                 let [result] = await this.DB.execute(`INSERT INTO plants (name) VALUES (?)`, [nameTree]);
//                 plantId = result.insertId;
//             }

//             await this.DB.execute(`INSERT INTO trees (id_plants, data) VALUES (?, ?)`, [plantId, new Date()]);
//         } catch (error) {
//             console.error("Error in createTree:", error);
//             throw error;
//         }
//     }
// }

// module.exports = Tree;

module.exports = Tree;
