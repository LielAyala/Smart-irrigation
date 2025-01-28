//בונה API 
//מייבים לפה ספריית אקספרס 
const express = require('express');

///במקום APP רושמים ROUTES
const router=express.Router();

//מתחיל לבנות ניתוב 
router.get('/',(req,res)=>{
    const {temp,light,moisture}=req.query;

    console.log(req.query);

    console.log(light);
    console.log(moisture);
  
})




module.exports=router;