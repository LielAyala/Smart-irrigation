//בונה API 
//מייבים לפה ספריית אקספרס 
const express = require('express');

///במקום APP רושמים ROUTES
const router=express.Router();

//מתחיל לבנות ניתוב 
router.get('/',(req,res)=>{
    console.log("hii");
  
})




module.exports=router;