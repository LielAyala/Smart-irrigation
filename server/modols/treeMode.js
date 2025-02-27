// treeModel.js
class Tree {
    constructor(db) {
        this.DB = db;
    }

    // פונקציה שמאחסנת את נתוני ה-ESP במסד הנתונים
    async storeESPData(temp, light, moisture) {
        await this.DB.execute(
            `INSERT INTO esp_data (temperature, light, moisture, timestamp) VALUES (?, ?, ?, ?)`,
            [temp, light, moisture, new Date()]  // שליחה למסד הנתונים
        );
    }

    // פעולות אחרות שקשורות לעצים...
    async getAllTree() {
        let [sql, l] = await this.DB.execute(`SELECT id FROM threes, plants WHERE id_plants = id`);
    }

    async createTree(nameTree) {
        let [sql, l] = await this.DB.execute(`SELECT id FROM plants WHERE name = ?`, [nameTree]);
        if (sql.length > 0) {
            await this.DB.execute(`INSERT INTO threes(id_plants, data) VALUES(?, ?)`, [sql[0].id, new Date()]);
        } else {
            sql = await this.DB.execute(`INSERT INTO plants(name) VALUES(?)`, [nameTree]);
            await this.DB.execute(`INSERT INTO threes(id_plants, data) VALUES(?, ?)`, [sql.insertId, new Date()]);
        }
    }

    async checkTreeExists(nameTree) {
        const [result] = await this.DB.execute(`SELECT id FROM plants WHERE name = ?`, [nameTree]);
        return result.length > 0;
    }

    async updateTree(id, name) {
        const [result] = await this.DB.execute(`UPDATE plants SET name = ? WHERE id = ?`, [name, id]);
    }

    async deleteTree(id) {
        const [result] = await this.DB.execute(`DELETE FROM threes WHERE id_plants = ?`, [id]);
    }
}





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
