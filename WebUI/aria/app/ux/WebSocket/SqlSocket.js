//Ext.Loader.setConfig ({
//                          enabled: true,
//                          paths: {
//                              'Ext.ux.WebSocket': 'app/ux/WebSocket/WebSocket.js'
//                          }
//                      });
Ext.define ('Ext.ux.SqlSocket', {
                extend : 'Ext.ux.WebSocket',
                alias  :  'ux.sqlsocket',
                xtype  :  'xsqlsocket',
                //    requires: [
                //        'Ext.ux.WebSocket'
                //    ],
                keySearch : function(x,y,callback){
                    var getList = function(v){
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
                    };
                    var keys = getList(x);
                    var fields = getList(y);
                    if(typeof callback == 'function')
                        for (field in y){
                            var key = keys.filter(function(z){return (JSON.stringify(z).toUpperCase() === JSON.stringify(field).toUpperCase())})[0];
                            var val;
                            if(Array.isArray(y))
                                val=key;
                            else
                                val=y[field];
                            val.name=field;
                            callback(key, val);
                        }
                },
                sql : function(proc,input,fields,result,cashTime){
                    var me = this;
                    proc=proc.toUpperCase();
                    var errors = [];
                    var query='';
                    var returns=[];
                    var p;
                    if(proc==='DB_PROCEDURES'){
                        p = {inputs : {},
                            outputs : {"PROCEDURE_NAME":{"pos":0,"type":"VARYING","length":124},
                                "INPUT_FIELDS":{"pos":1,"type":"VARYING","length":24000},
                                "RETURN_FIELDS":{"pos":2,"type":"VARYING","length":24000}}};
                    }
                    else {
                        p = this.db.procedures[proc];
                        if(typeof p == 'undefined'){
                            console.log('Can not finde sql procedure ', proc);
                            return;
                        }
                    }
                    this.keySearch(fields,p.outputs,function(key,val){
                        returns[val.pos]=key});
                    returns=returns.filter(function(v){return v;});
                    if(returns.length==0)
                        returns=['*'];
                    if(typeof p.outputs == 'undefined')
                        query='EXECUTE PROCEDURE '+proc;
                    else{
                        if(p.outputs.length===0)
                            query='EXECUTE PROCEDURE '+proc;
                        else
                            query='SELECT '+returns.join()+' FROM '+proc;
                    }
                    var param=[];
                    var error = {};
                    switch(typeof input){
                    case 'string':
                    case 'number':
                        var inArr = Object.keys(p.inputs);
                        if(inArr.length>0)
                            param[0]=input;
                        break
                    case 'object':
                        me.keySearch(input,p.inputs,function(key,val){
                            var value;
                            if(val.type === 'TIMESTAMP')
                                value=input[key].toLocaleString("ru-RU");
                            else
                                value=input[key];
                            if(typeof input[key] == 'undefined'){
                                if(typeof val.default != 'undefined')
                                    value=val.default;
                                else{
                                    error = {};
                                    error.description = Ext.String.format('Missing parameter {0} in procedure {1}', val.name,proc);
                                    error.string='missing parameter';
                                    error.info={procedure:proc, parameter:val.name};
                                    errors.push(error);
                                }
                            }
                            param[val.pos]=value;
                        });
                        break
                        //             case 'function':
                        //                 result = input;
                        //                 break
                    default:
                        error.description = 'Input is not an object';
                        error.string='input is not an object';
                        error.info={input:input};
                        errors.push(error);
                    }

                    ////TODO param objecct key/val, interselect keys
                    if(param.length!=Object.keys(p.inputs).length){
                        error.description = 'Not enough parameters';
                        error.string='not enough parameters';
                        error.info={input:input,values:param,expected:p.inputs};
                        errors.push(error);
                    }
                    if(param.length!=0)
                        query=query+"('"+param.join("','")/*.replace(/,/g,"','")*/+"')";
                    if((typeof result == 'function') && (typeof result == 'object')){
                        error.description = 'Result value is not an object or function';
                        error.string='result value is not an object or function';
                        error.info={result:result};
                        errors.push(error);
                    }

                    if(errors.length===0){
                        if(typeof cashTime == 'undefined')
                            cashTime=0;
                        this.db.cashedQuery(query, cashTime, function(resp){
                            var rows = JSON.parse(resp);
                            var res={};
                            if(rows.length===1)
                                res=rows[0];
                            else
                                res=rows;
                            //var res = JSON.parse(resp);
                            if ('RESULT' in res)
                                try {
                                    res=JSON.parse(res.RESULT);
                                } catch (e) {
                                    errors.push({description:'exception in RESULT',
                                                    string:'exception in RESULT',
                                                    info:e});
                                }
                            if((typeof result === 'undefined') && (typeof fields !== 'undefined')){
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
                                        }else
                                            Object.keys(res).every(function(key){return input[key]=res[key]});
                                    }
                                }
                                //TODO error handling
                                return;
                            }
                            if(typeof res === 'undefined'){
                                errors.push({description:'typeof res is undefined ' + query,
                                                string:'typeof res is undefined ' + query,
                                                info:RESULT});
                            } else {
                                if(res.length === 2){
                                    if(res[1] === "Error")
                                        errors.push({description:'Error in sql statement ' + query,
                                                        string:'Error in sql statement ' + query,
                                                        info:res[0]});
                                }
                            }
                            if(errors.length===0){
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
                                    }
                                    //else
                                    //    Object.keys(res).every(key=>result[key]=res[key]);
                                }
                            }else{
                                //errors print
                                console.log('Error in SQL res!!!',JSON.stringify(errors));
                            }
                        });

                    }
                    else{
                        //errors print
                        console.log('Error in SQL fnc!!!',JSON.stringify(errors));
                    }
                },
                listeners: {
                    open: function(ws){
                        console.log('SqlSocket.js opened');
                        this.sql('DB_PROCEDURES','',['PROCEDURE_NAME', 'INPUT_FIELDS', 'RETURN_FIELDS'],
                                 function(procs){
                                     ws.db.procedures={};
                                     for(i in procs){
                                         try{
                                             var inputs = JSON.parse(procs[i].INPUT_FIELDS.replace(/\\/g,'').replace(/'/g,'"').replace(/NULL/g,'null'));
                                             var outputs = JSON.parse(procs[i].RETURN_FIELDS.replace(/\\/g,'').replace(/'/g,'"').replace(/NULL/g,'null'));
                                         }
                                         catch(e){
                                             console.log(e,' in procedure',procs[i]);
                                         }
                                         ws.db.procedures[procs[i].PROCEDURE_NAME]={
                                             inputs : inputs,
                                             outputs : outputs
                                         };
                                     }
                                     //console.log('ws.db.procedures',ws.db.procedures);
                                     ws.fireEvent('ready', ws);
                                 });
                    }
                },
                constructor : function() {
                    this.callParent(arguments);
                }
            });
