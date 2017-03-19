Ext.define ('Ext.ux.SqlSocket', {
    extend : 'Ext.ux.WebSocket',
    getList : function(v){
        switch (typeof v) {
        case 'string':
        case 'number':
            var a=[];
            a.push(v);
            return a;
        case 'array':
            return v; //??? when
        case 'object':
            if(Array.isArray(v))
                return v;
            else
                return Object.keys(v);
        default :
            return [];
        }
    },
    keySearch : function(x,y,callback){
        var keys = getList(x);
        var fields = getList(y);
        if(typeof callback == 'function')
            for (field in y){
                //var key = keys.filter(z => JSON.stringify(z).toUpperCase() === JSON.stringify(field).toUpperCase())[0];
                var val;
                if(Array.isArray(y))
                    val=key;
                else
                    val=y[field];
                    val.name=field;
                callback(key, val);
        }
    },
    db  : {procedures:[],query2json:function(){}},
    sql : function(proc,input,fields,result){
             proc=proc.toUpperCase();
             var errors = [];
             var query='';
             var returns=[];
             var p = this.db.procedures[proc];
             if(typeof p == 'undefined'){
                 console.log('Can not finde sql procedure ', proc);
                 return;
             }
             keySearch(fields,p.outputs,function(key,val){
                 returns[val.pos]=key});
             //returns=returns.filter(v => v);
             if(returns.length==0)
                 returns=['*'];
             if(typeof p.outputs == 'undefined')
                 query='EXECUTE PROCEDURE '+proc;
             else{
                 if(p.outputs.length==0)
                     query='EXECUTE PROCEDURE '+proc;
                 else
                     query='SELECT '+returns.join()+' FROM '+proc;
             }
             var param=[];

             switch(typeof input){
             case 'string':
             case 'number':
                 var inArr = Object.keys(p.inputs);
                 if(inArr.length>0)
                   param[0]=input;
                 break
             case 'object':
                 keySearch(input,p.inputs,function(key,val){
                     var value;
                     if(val.type == 'TIMESTAMP')
                         value=input[key].toLocaleString("ru-RU");
                     else
                       value=input[key];
                     if(typeof input[key] == 'undefined'){
                         if(typeof val.default != 'undefined')
                             value=val.default;
                         else{
                             var error = {};
                             error.description = Ext.String.format('Missing parameter {0} in procedure {1}', val.name,proc);
                             error.string='missing parameter';
                             error.info={procedure:proc, parameter:val.name};
                             errors[errors.length]=error;
                         }
                     }
                     param[val.pos]=value;
                 });
                 break
             default:
                 var error = {};
                 error.description = 'Input is not an object';
                 error.string='input is not an object';
                 error.info={input:input};
                 errors[errors.length]=error;
             }

             ////TODO param objecct key/val, interselect keys
             if(param.length!=Object.keys(p.inputs).length){
                 var error = {};
                 error.description = 'Not enough parameters';
                 error.string='not enough parameters';
                 error.info={input:input,values:param,expected:p.inputs};
                 errors[errors.length]=error;
             }
             if(param.length!=0)
                 query=query+"('"+param.join("','")/*.replace(/,/g,"','")*/+"')";
             if((typeof result == 'function') && (typeof result == 'object')){
                 var error = {};
                 error.description = 'Result value is not an object or function';
                 error.string='result value is not an object or function';
                 error.info={result:result};
                 errors[errors.length]=error;
             }

             if(errors.length==0){
                 ws.db.query2json(query, function(resp){
                               var rows = JSON.parse(resp);
                               var res={};
                               if(rows.length=1)
                res=rows[0];
                               else
                res=rows;
                               //var res = JSON.parse(resp);
                               if ('RESULT' in res)
                try {
                        res=JSON.parse(res.RESULT);
                    } catch (e) {
                        console.log(e,' in RESULT');
                    }
                               if(typeof result == 'undefined' && typeof fields != 'undefined'){
                if(typeof input == 'object'){
                    if(Array.isArray(input)){
                        if(Array.isArray(res)){
                            for (index in res)
                     if(res.hasOwnProperty(index))
                         input.push(res[index]);
                        }
                        else
                            input.push(res);
                    }
                    else{
                        if(Array.isArray(res)){
                            for (index in res)
                     if(res.hasOwnProperty(index))
                         for (key in res[index])
                             if(res[index].hasOwnProperty(key))
                                 input[key]=res[index][key];
                        }
                        //else
                        //    Object.keys(res).every(key=>input[key]=res[key]);
                    }
                }
                //TODO error handling
                return;
                               }
                               if(typeof result == 'function')
                result(res);
                               if(typeof result == 'object'){
                if(Array.isArray(result)){
                    if(Array.isArray(res)){
                        for (index in res)
                            if(res.hasOwnProperty(index))
                     result.push(res[index]);
                    }
                    else
                        result.push(res);
                }
                else{
                    if(Array.isArray(res)){
                        for (index in res)
                            if(res.hasOwnProperty(index))
                     for (key in res[index])
                         if(res[index].hasOwnProperty(key))
                             result[key]=res[index][key];
                    }
                    //else
                    //    Object.keys(res).every(key=>result[key]=res[key]);
                }
                               }
                           });

             }
             else
                 console.log('Error in SQL fnc!!!',JSON.stringify(errors));
         
                    }
                           });
