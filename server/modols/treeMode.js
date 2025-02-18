class Tree{
    constructor(db){
        this.DB=db;
    }
    async createTree(nameTree){
        let [sql,l]=await this.DB.execute(`SELECT id FROM plants whewe name='${nameTree}'`);
        console.log(sql);
        if(sql.length>0){
            await this.DB.execute(`INSERT INTO trees(id_plants,data)VALUE('?','?');`,[sql[0].id,new Date()]);
        }
        else{
            sql=await this.DB.execute(`INSERT INTO plants(name) VALUE('?');`,[nameTree]);
            console.log(sql);
            await this.DB.execute(`INSERT INTO trees(id_plants,data)VALUE('?','?');`,[sql.insertid,new Date()]);
        }
    }
}