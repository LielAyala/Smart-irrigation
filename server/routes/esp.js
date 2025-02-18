//בונה API 
const express = require('express');//מייבים לפה ספריית אקספרס 
const router=express.Router();///במקום APP רושמים ROUTES
const fs=require('fs');


//מתחיל לבנות ניתוב 
router.get('/',(req,res)=>{
    const {temp,light,moisture}=req.query;

    console.log(req.query);

    console.log(light);
    console.log(moisture);
  
})

//תחלופה לסוקט כי אין לנו זמן לעשות את זה 
router.get('/state',(req,res)=>{
    let data=JSON.parse(fs.readFileSync("inside_information.json","utf8"));
    data={state:data.state,
        data:new Data()
    }
    res.json(data);//גישה לKEY באובייקט 
})

router.get('/dataMode',(req,res)=>{
    //פונקציה גלובלית לטיפול במצבים 
    //שולחים כסטרינג ואם נירצה לשלוף יהיה יותר קל 
    const {state}=req.query;//לוקח את המצב 
    //נשלוף את הפרמטרים מהJSON
    //utf8 זה סגנון הטקסט 
    let data=JSON.parse(fs.readFileSync("inside_information.json","utf8"));
    //שליחת הבקשה לESP לאחר תרגום הJSON למספר 
    res.json(data[state]);//גישה לKEY באובייקט 

})



module.exports=router;