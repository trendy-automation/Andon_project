var store = Ext.create('Ext.data.TreeStore', {
    root: {
        text: "/",
        expanded: true,
        children: [
            { text: "detention", leaf: true },
            { text: "homework", expanded: true, children: [
                { text: "book report", leaf: true },
                { text: "alegrbra", leaf: true}
            ] },
            { text: "buy lottery tickets", leaf: true }
        ]
    }
});

// Ext.create('Ext.tree.Panel', {
//     title: 'Simple Tree',
//     width: 200,
//     height: 150,
//     store: store,
//     rootVisible: true,
//     renderTo:
// });

Ext.define('AndonPortal.view.Tree', {
    //extend: 'Ext.panel.Panel',
    extend: 'Ext.tree.Panel',
    alias:  'widget.mysimpletree',
    viewConfig: {plugins: {ptype: 'treeviewdragdrop'}}, 
    renderTo: Ext.getBody(),
    title: 'TreeGrid',
    rootVisible: true,
    hideCollapseTool: false,
    collapsible: true,
    width: 300,
    height: 150,
    fields: ['name', 'description'],
    columns: [{
        xtype: 'treecolumn',
        text: 'Name',
        dataIndex: 'name',
        width: 150,
        sortable: true
    }, {
        text: 'Description',
        dataIndex: 'description',
        flex: 1,
        sortable: true
    }],
    root: {
        name: 'Root',
        //description: 'Root description',
        //leaf:false,
        children: [{
            name: 'Child 1'//,
            //description: 'Description 1'
        }, {
            name: 'Child 2'//,
            //description: 'Description 2'
        }]
    }
});
