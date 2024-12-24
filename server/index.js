/* ייבוא של Express*/
const express = require('express');

/* יצירת אפליקציה ב-Express*/
const app = express();

/* הגדרת הפורט*/
const PORT = 3001;

/* יצירת נתיב בסיסי לבדיקה*/
app.get('/', (req, res) => {
    res.send('השרת עובד כמו שצריך! 🚀');
});

/*האזנה לפורט*/
app.listen(PORT, () => {
    console.log(`raning${PORT}`);
});
