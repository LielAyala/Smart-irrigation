const Tree=require('../modols/treeMode');
const db=require('../modols/dataBase');

const tree=new Tree(db);

router.get("/add",(req,res)=>{
    try{
        const {name}=req.bady;
        tree.createTree(name);
        res.status(201).json({massage:"ctreate tree succes"})
    }
    catch(error){
        console.log(error);
        res.status(500).json({massage:""})
    }
    
    tree.createTree("mango");
})
module.exports=router;