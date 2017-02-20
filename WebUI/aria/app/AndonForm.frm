VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} AndonForm 
   Caption         =   "AndonSetup"
   ClientHeight    =   10380
   ClientLeft      =   45
   ClientTop       =   375
   ClientWidth     =   19110
   OleObjectBlob   =   "AndonForm.frx":0000
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "AndonForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

'##########Treeview Code##########
'Add this to your form's declaration section
Private WithEvents mcTree As clsTreeView
Attribute mcTree.VB_VarHelpID = -1
Private WithEvents mcTree2 As clsTreeView
Attribute mcTree2.VB_VarHelpID = -1

Private mbExit As Boolean    ' to exit a SpinButton event
'/##########Treeview Code##########

'''' for stress testing this demo
Private mlCntChildren As Long
Private mlDemoNo As Long

Public AppName As String
Public DebugMode As Long

Public DragTypeId As Long
Public DragObjectId As Long

Public ErrorDescription As String
Public ErrorNumber As Integer



#If Mac Then
    Const mcPtPixel As Long = 1
#Else
    Const mcPtPixel As Single = 0.75
#End If

Private Sub InitializeExtTree(TreeName As String)
Dim FBcon As ADODB.Connection
Dim Fbcmd As ADODB.command
Dim FbRst As ADODB.Recordset
Dim nKey As String
Dim RootPath As String
Dim cRoot As clsNode
     On Error GoTo errH
    AppName = "InitializeExtTree"
    
    Call DB_CONNECT(FBcon, Fbcmd)
    Set mcTree2 = New clsTreeView
    InitializeTreeView mcTree2, Me.frTreeControl2
        Fbcmd.CommandText = "SELECT NODE_ID, PID, OBJECT_NAME, OBJECT_ID, OBJECT_TYPE_ID" & _
                                " TYPE_TABLE, PKEY FROM " & TreeName
        Set FbRst = Fbcmd.execute
        'Rcrdst2TreeView mcTree2, FbRst, Replace(Replace(TreeName, "TREE_SELECT_TREEVIEW('TREE_", ""), "')", ""), True
        Rcrdst2TreeView mcTree2, FbRst, TreeName, True
    With mcTree2
        If (.RootNodes.count > 0) Then
            mcTree2.RootNodes(1).Expanded = True
            .ExpandNode .RootNodes(1)
        End If
        .NodeHeight = 16
        .refresh
    End With
    TDBGrid1.visible = False
    frTreeControl2.visible = True
FBcon.Close
Set FBcon = Nothing

Exit Sub

errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
    If Not mcTree Is Nothing Then
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
        mcTree.NodesClear
        mcTree.TerminateTree
    End If
End Sub

Private Sub InitializeTrees()
Dim FBcon As ADODB.Connection
Dim Fbcmd As ADODB.command
Dim FbTreesViewRst As ADODB.Recordset
Dim FbRst As ADODB.Recordset
Dim nKey As String
Dim RootPath As String
Dim cRoot As clsNode
'Dim cNode As clsNode
'Dim cCurNode As clsNode
'Dim cExtraNode As clsNode
'Dim i As Integer
     On Error GoTo errH
    AppName = "InitializeTrees"
    
Call DB_CONNECT(FBcon, Fbcmd)
      'Fbcmd.CommandText = "SELECT '''' || TREE_NAME || '''' AS TREE_NAME FROM TREE_SELECT_TREE " & _
      '  "UNION ALL " & _
      '  "SELECT FIRST 1 '''plant'',2' AS TREE_NAME FROM TREE_SELECT_TREE"
      Fbcmd.CommandText = "SELECT TREE_NAME FROM TREE_SELECT_TREE " & _
      "UNION ALL SELECT FIRST 1 'INTERFACE_PREVIEW' AS TREE_NAME FROM TREE_SELECT_TREE " & _
      "UNION ALL SELECT FIRST 1 'SMS_PREVIEW' AS TREE_NAME FROM TREE_SELECT_TREE"
      
Set FbTreesViewRst = Fbcmd.execute

    Set mcTree = New clsTreeView
    InitializeTreeView mcTree, Me.frTreeControl
    
    With mcTree
    
    While Not FbTreesViewRst.EOF
        'Fbcmd.CommandText = "SELECT NODE_ID, PID, OBJECT_NAME, OBJECT_ID, OBJECT_TYPE_ID," & _
        '                        " TYPE_TABLE FROM TREE_SELECT_TREEVIEW(" & _
        '   FbTreesViewRst.Fields(0).Value & ")"
        Fbcmd.CommandText = "SELECT NODE_ID, PID, OBJECT_NAME, OBJECT_ID, OBJECT_TYPE_ID," & _
                                " TYPE_TABLE FROM TREE_SELECT_TREEVIEW('" & _
            FbTreesViewRst.Fields(0).Value & "')"
        If (FbTreesViewRst.Fields(0).Value = "INTERFACE_PREVIEW") Then
            Fbcmd.CommandText = "SELECT NODE_ID, PID, OBJECT_NAME, OBJECT_ID, OBJECT_TYPE_ID," & _
                                    "TYPE_TABLE FROM USER_INTERFACE_PREVIEW"
        End If
        If (FbTreesViewRst.Fields(0).Value = "SMS_PREVIEW") Then
            Fbcmd.CommandText = "SELECT NODE_ID, PID, OBJECT_NAME, OBJECT_ID, OBJECT_TYPE_ID," & _
                                    "TYPE_TABLE FROM USER_SMS_PREVIEW"
        End If
        Set FbRst = Fbcmd.execute
        Rcrdst2TreeView mcTree, FbRst, FbTreesViewRst.Fields(0).Value
        FbTreesViewRst.MoveNext
    Wend
    Fbcmd.CommandText = "SELECT MAX(ID_OBJECT_TYPE), LIST(OBJECT_TYPE_NAME), TYPE_TABLE FROM OBJECT_SELECT_TYPES " & _
                            "GROUP BY TYPE_TABLE"
    Set FbRst = Fbcmd.execute
    While Not FbRst.EOF
            Set cRoot = .AddRoot("ObjectType_" & FbRst.Fields(0).Value, _
                            Replace(FbRst.Fields(2).Value, "TBL_", ""), "Scroll", "Scroll")
                With cRoot
                   '.TreeName = ""
                   '.NodeId =
                   '.Pid =
                   .Expanded = False
                   .ObjectId = FbRst.Fields(0).Value
                   .ObjectTypeId = FbRst.Fields(0).Value
                   .TypeTable = FbRst.Fields(2).Value
                   .RootPath = ""
                End With
                FbRst.MoveNext
            Wend
        .NodeHeight = 16
        .refresh
    End With
FBcon.Close
Set FBcon = Nothing

Exit Sub

errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
    If Not mcTree Is Nothing Then
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
        mcTree.NodesClear
        mcTree.TerminateTree
    End If
End Sub

Private Sub InitializeTreeView(ByRef newMcTree As clsTreeView, ByRef frTreeControl As Control)
         On Error GoTo errH
    AppName = "InitializeTreeView"

'    Set newMcTree = New clsTreeView
    With newMcTree
            Set .TreeControl = frTreeControl
        .AppName = Me.AppName
        .CheckBoxes = False
        .RootButton = True
        .EnableLabelEdit(bAutoSort:=False, bMultiLine:=False) = True
        .FullWidth = True
        '.Indentation = True * mcPtPixel
        '.NodeHeight = True * mcPtPixel
        .ShowLines = True
        mbExit = True
        '.ShowExpanders = True
        mbExit = False
        Set .Images = Me.frmImageBox
    End With
Exit Sub

errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
    If Not mcTree Is Nothing Then
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
        mcTree.NodesClear
        mcTree.TerminateTree
    End If
End Sub


Private Sub Rcrdst2TreeView(ByRef mcTree As clsTreeView, ByRef FbRst As Recordset, TreeName As String, Optional Expanded As Boolean)
Dim nKey As String
Dim RootPath As String
Dim cRoot As clsNode
Dim cNode As clsNode
Dim cCurNode As clsNode
Dim cExtraNode As clsNode
Dim i As Integer
     On Error GoTo errH
    AppName = "Rcrdst2TreeView"
    i = 1
        Set cRoot = mcTree.AddRoot(TreeName, TreeName, "FolderClosed", "FolderOpen")
            cRoot.Bold = True
            cRoot.ControlTipText = TreeName
            cRoot.RootPath = ""
            cRoot.TreeName = TreeName
            cRoot.Expanded = False
            Set cCurNode = cRoot
            While Not FbRst.EOF
                If (IsNull(FbRst.Fields(1).Value)) Then
                    Set cCurNode = cRoot
                    RootPath = ""
                Else
                    nKey = TreeName & "_" & FbRst.Fields(1).Value
                    For Each cNode In mcTree.Nodes
                        If cNode.key = nKey Then
                            Set cCurNode = cNode
                            Exit For
                        End If
                    Next
                End If
                    nKey = TreeName & "_" & FbRst.Fields(0).Value
                For Each cNode In mcTree.Nodes
                    If cNode.key = nKey Then
                        nKey = nKey & "-" & i
                        i = i + 1
                        Exit For
                    End If
                Next
                    
                Set cExtraNode = cCurNode.AddChild(nKey, FbRst.Fields(2).Value)
                With cExtraNode
                   .TreeName = TreeName
                   .NodeId = FbRst.Fields(0).Value
                   .Pid = FbRst.Fields(1).Value
                   .ObjectId = FbRst.Fields(3).Value
                   .ObjectTypeId = FbRst.Fields(4).Value
                   .TypeTable = IIf(IsNull(FbRst.Fields(5).Value), "", FbRst.Fields(5).Value)
                   .Expanded = Expanded
                   .RootPath = IIf(cCurNode.RootPath = "", "", cCurNode.RootPath & ",") & cExtraNode.NodeId
                    If (FbRst.Fields.count = 6) Then
                        If (FbRst.Fields(5).name = "PKEY") Then
                            .ObjectId = FbRst.Fields(5).Value
                        End If
                    End If
                End With
                FbRst.MoveNext
            Wend
    Exit Sub
errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
    If Not mcTree Is Nothing Then
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
        mcTree.NodesClear
        mcTree.TerminateTree
    End If
End Sub

Private Sub cmdDelFromTree_Click()
RemoveNode mcTree.ActiveNode
End Sub

Private Sub CommandButton1_Click()
Dim MSTSC As Object
MSTSC = Shell("C:\windows\system32\mstsc.exe /admin /v:" & "10.208.102.70", 1)
End Sub

Private Sub frTreeControl2_Click()
    If Not mcTree2 Is Nothing Then
        mcTree2.EnterExit False
        If Not mcTree2.ActiveNode Is Nothing Then
            'mcTree2_Click mcTree2.ActiveNode
        End If
    End If
End Sub

Private Sub frTreeControl2_DblClick(ByVal Cancel As MSForms.ReturnBoolean)
Dim cParentNode As clsNode
Dim cNode As clsNode
Dim i As Integer
    AppName = "frTreeControl2_DblClick"
    Set cParentNode = mcTree.ActiveNode
    Set cNode = mcTree2.ActiveNode
    If cParentNode Is Nothing Then Exit Sub
    If cNode Is Nothing Then Exit Sub
    On Error GoTo errH
    AddNode cParentNode, mcTree2.RootNodes(1).Caption, cNode.ObjectId
    Exit Sub
errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
        ErrorDescription = Err.Description
        ErrorNumber = Err.Number
        MsgBox ErrorDescription, , AppName
        ErrorLog ErrorDescription, ErrorNumber, AppName
End Sub

Private Sub frTreeControl2_Enter()

End Sub

Private Sub SQLProcFrame_Click()

End Sub

Private Sub TDBGrid1_KeyDown(KeyCode As Integer, ByVal Shift As Integer)
Dim sMsg As String
AppName = "TDBGrid1_KeyDown"
Select Case KeyCode
    Case vbKeyDelete         ' Delete
        If Not TDBGrid1.EditActive Then
            sMsg = "Are you sure you want to delete record '" & TDBGrid1
            If MsgBox(sMsg, vbOKCancel, AppName) <> vbOK Then
                'restrict
                TDBGrid1.AllowDelete = False
                'TDBGrid1.Cancel = True
                'Exit Sub
            End If
        Else
            TDBGrid1.AllowDelete = False
        End If
    Case vbKeyX, vbKeyX + 48
        If Shift = 2 Then    ' Ctrl-X move
            TDBGrid1.AllowDelete = False
        End If

End Select

End Sub





Private Sub TDBGrid1_KeyUp(KeyCode As Integer, ByVal Shift As Integer)
Select Case KeyCode
    Case vbKeyDelete
        TDBGrid1.AllowDelete = True
End Select
End Sub

Private Sub TDBGrid1_RowResize(Cancel As Integer)
    TDBGrid1.FilterBar = False
End Sub

Private Sub UserForm_Initialize()
Dim xlBook As Workbook

For Each xlBook In Excel.Application.Workbooks
    If InStr(1, xlBook.name, "Andon_setup", vbTextCompare) > 0 Then
        Application.Workbooks(xlBook.name).Activate
        Exit For
    End If
Next

DebugMode = 1
'see the Compile constant DebugMode in tools, vbaproject properties
'DebugMode=1 will enable the #If to Stop in Error handlers
    
    ' Hide the Image container
    Me.frmImageBox.visible = False
    Me.frmImageBox.Enabled = False

         Me.frTreeControl.Font.size = 12
    mbExit = False
    
    TabTypes_Clear
    LoadTrees
End Sub

Private Sub UserForm_Terminate()
  
    #If DebugMode = 1 Then
        gFormTerm = gFormTerm + 1
        ClassCounts
    #End If
End Sub

Private Sub LoadTrees()
    InitializeTDBGrid
    InitializeTrees
    If Not mcTree Is Nothing Then
        mbExit = True
        mbExit = False
        Me.frTreeControl.SetFocus
    End If
End Sub
   
Private Sub InitializeTDBGrid()
        'TDBGrid1
            TDBGrid1.height = 420
            TDBGrid1.width = 600
            TDBGrid1.DataSource = TData1
            TData1.CommandType = adCmdTable
            TData1.connectionString = GetConnectionString()
            'TData1.SafeMode = True
            TData1.RecordSource = "tbl_plant_objects"
            TData1.RetrieveFields
            TData1.refresh
            TDBGrid1.AllowAddNew = True
            TDBGrid1.AllowArrows = True
            TDBGrid1.AllowDelete = True
            TDBGrid1.MarqueeUnique = False
            'TDBGrid1.AllowColSelect = False
            TDBGrid1.MarqueeStyle = dbgNoMarquee
            'TDBGrid1.SelectedStyle
            TDBGrid1.AllowUpdate = True
            TDBGrid1.WrapCellPointer = True
            TDBGrid1.Font.size = 12
            TDBGrid1.HeadFont.size = 12
            TDBGrid1.rowHeight = 20
            TDBGrid1.MultiSelect = dbgMultiSelectExtended
            TDBGrid1.AllowRowSelect = False
            TDBGrid1.FilterBar = True
            TDBGrid1.FilterActive = True
            'TDBGrid1.GroupColumns
            TDBGrid1.DataView = dbgGroupView
            TDBGrid1.AllowRowSizing = True
            TDBGrid1.AllowColMove = True
            TDBGrid1.AllowColSelect = True
            TDBGrid1.Appearance = dbg3D
            TDBGrid1.SelectedBackColor = &HFF0000    '&H80000003
            TDBGrid1.EditBackColor = vbWhite
            TDBGrid1.EditForeColor = vbBlack
            'TDBGrid1.AddNewMode = dbgAddNewCurrent
            TDBGrid1.columns.item(0).AllowSizing = True
            TDBGrid1.columns.item(0).AutoSize
            
            
            
            
            'TDBGrid1.SelectedForeColor = vbBlack
            'TDBGrid1.SelectedStyle =
            


        'ProcCombo
            ControlFullFill "SELECT PROCEDURE_STRING FROM DB_GET_PROCEDURES", "ProcCombo"

End Sub

Private Sub TDBGrid1_AfterInsert()
'    GridUpdate
End Sub

Private Sub TDBGrid1_DblClick()
If (TDBGrid1.FirstRow <> "" And Not (TDBGrid1.EditActive) And TDBGrid1.SelBookmarks.count = 1) Then
    AddFromGrid (TDBGrid1.FirstRow + TDBGrid1.Row)
End If
End Sub

Private Sub TDBGrid1_DragCell(ByVal SplitIndex As Integer, RowBookmark As Variant, ByVal ColIndex As Integer)
AppName = "TDBGrid1_DragCell"
    On Error GoTo errH
'DragTypeId = TypeCombo.list(TypeCombo.ListIndex, 0)
If Not (mcTree Is Nothing) Then
    If Not (mcTree.ActiveNode Is Nothing) Then
        DragTypeId = mcTree.ActiveNode.ObjectTypeId
        If (left(TDBGrid1.columns(0).Caption, 3) = "ID_" Or Right(TDBGrid1.columns(0).Caption, 3) = "_ID") Then
            DragObjectId = TDBGrid1.columns(0).CellValue(RowBookmark)
        Else
            DragObjectId = 0
        End If
    End If
End If
    Exit Sub

errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
End Sub

Private Sub TDBGrid1_Error(ByVal DataError As Integer, Response As Integer)
MsgBox DataError, , Response
End Sub
Private Sub TDBGrid1_GroupColMove(ByVal Position As Integer, ByVal ColIndex As Integer, Cancel As Integer)
    Dim strSort As String
    Dim col As TrueOleDBGrid80.column
    ' Loop through GroupColumns collection and construct the sort string for
    ' the Sort property of the Recordset.
    For Each col In TDBGrid1.GroupColumns
        If strSort <> vbNullString Then
            strSort = strSort & ", "
        End If
        strSort = strSort & "[" & col.DataField & "]"
    Next col
    TDBGrid1.HoldFields
    TData1.SourceRecordset.Sort = strSort
End Sub
 
Private Function getFilter() As String
Dim col As TrueOleDBGrid80.column
Dim cols As TrueOleDBGrid80.columns
    ' Creates the SQL statement in adodc1.recordset.filter and only filters
    ' text currently. It must be modified to filter other data types.
    Dim tmp As String
    Dim n As Integer
        Set cols = TDBGrid1.columns
    For Each col In cols
        If Trim(col.FilterText) <> "" Then
            n = n + 1
            If n > 1 Then
                tmp = tmp & " AND "
            End If
            tmp = tmp & col.DataField & " LIKE '" & col.FilterText & "*'"
        End If
    Next col
    getFilter = tmp
End Function

Private Sub GridUpdate(Optional TypeTable As String)
Dim i As Integer

AppName = "GridUpdate"
        On Error GoTo errH
        
    If Not (IsNull(TypeTable) Or TypeTable = Empty) Then
        If (InStr(TypeTable, "TREE_SELECT_TREEVIEW('TREE_") > 0) Then
            InitializeExtTree TypeTable
            Exit Sub
        Else
            TDBGrid1.visible = True
            Me.frTreeControl2.visible = False
        End If
        
        If (TData1.RecordSource <> TypeTable) Then
            'frTreeControl2.visible = False
            'TDBGrid1.visible = True
            
            
            TDBGrid1.ClearSelCols
            TDBGrid1.ClearFields
            'TDBGrid1.SpringMode = True
            TDBGrid1.DataView = dbgNormalView
            
            While TDBGrid1.Splits.count > 1
                TDBGrid1.Splits.remove (0)
            Wend
            TDBGrid1.DataView = dbgGroupView
            'TDBGrid1.MarqueeStyle = dbgHighlightCell
            'TDBGrid1.SpringMode = False
            'While TDBGrid1.columns.count > 0
            '    TDBGrid1.columns.remove 0
            'Wend
            
            'TDBGrid1.refresh
            
            'проверить наличие таблицы
            TData1.RecordSource = TypeTable
            TableLabel = TypeTable
        End If
    End If
        
        TData1.refresh
        TData1.RetrieveFields
        TData1.refresh
        'TDBGrid1.refresh

        'TData1.RetrieveFields
        If (left(TDBGrid1.columns(0).Caption, 3) = "ID_") Then
            TData1.Fields(0).Required = False
            TDBGrid1.columns(0).ConvertEmptyCell = dbgAsNull
        End If
        i = 0
        While TDBGrid1.columns.count > i
            TDBGrid1.columns.item(i).AllowSizing = True
            TDBGrid1.columns.item(i).AutoSize
            i = i + 1
        Wend
        
        TDBGrid1.FilterBar = True

    'End If
Exit Sub
errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
End Sub

Private Sub TDBGrid1_FilterChange()
Dim cols As TrueOleDBGrid80.columns
    'Gets called when an action is performed on the filter bar.
    On Error GoTo errHandler
    Set cols = TDBGrid1.columns
    Dim c As Integer
    c = TDBGrid1.col
    TDBGrid1.HoldFields
    TData1.SourceRecordset.Filter = getFilter()
    TDBGrid1.col = c
    TDBGrid1.EditActive = True
    TDBGrid1.refresh
Exit Sub
errHandler:
    MsgBox Err.Source & ":" & vbCrLf & Err.Description
    'Call cmdClearFilter_Click
End Sub

Private Sub ProcCombo_Change()
    LblResult.Caption = ""
End Sub

Private Sub TabTypes_Click()
    If TabTypes.NumTabs > 0 Then
        GridUpdate TabTypes.TabData(TabTypes.CurrTab)
    End If
End Sub

Private Sub TabTypes_Clear()
        While TabTypes.NumTabs > 0
            TabTypes.RemoveTab 0
        Wend
End Sub

Private Sub ControlFullFill(Source As String, Target As String)
Dim FBcon As ADODB.Connection
Dim Fbcmd As ADODB.command
Dim FbObjectsRst As ADODB.Recordset
AppName = "ControlFullFill"
    On Error GoTo errH
Call DB_CONNECT(FBcon, Fbcmd)
Fbcmd.CommandText = Source
Set FbObjectsRst = Fbcmd.execute
Rcrdst2Control FbObjectsRst, Target
FBcon.Close
Set FBcon = Nothing
Exit Sub
errH:
    If DebugMode = 1 Then
'        Stop
'        Resume
    End If
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
End Sub

Private Sub Rcrdst2Control(SQLRcrdst As Recordset, ByVal Control_name As String)
Dim i As Integer
    On Error GoTo errH
    AppName = "Rcrdst2Control"
   With Me.Controls(Control_name)
     If .name = "TabTypes" Then
        TabTypes_Clear
     Else
        .clear
        .Locked = True
     End If
     If Not (SQLRcrdst Is Nothing) Then
     If SQLRcrdst.BOF Or SQLRcrdst.EOF Then
        Exit Sub
     End If
      SQLRcrdst.MoveFirst
      '.AddItem "Выбирите " & IIf(Control_name = "TypeCombo", "тип", "объект")
      Do While Not SQLRcrdst.EOF
        If .name = "TabTypes" Then
            .AddTab CStr(SQLRcrdst.Fields(0))
            '.CurrTab.tag = CStr(SQLRcrdst.Fields(0))
            .TabData(.NumTabs - 1) = CStr(SQLRcrdst.Fields(0))
            'TabTypes.GetTab(TabTypes.NumTabs) .CurrTab.tag
            'TabTypes.TabData (0)
        Else
            .AddItem CStr(SQLRcrdst.Fields(0))
            For i = 1 To SQLRcrdst.Fields.count - 1
                .list(.ListCount - 1, i) = CStr(SQLRcrdst.Fields(i).Value)
            Next i
        End If
        SQLRcrdst.MoveNext
      Loop
        If Not .name = "TabTypes" Then
          If (.ListCount > 0) Then
            .ListIndex = 0
          End If
          .Locked = False
        Else
          .CurrTab = 0
        End If
      End If
   End With
Exit Sub
errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
End Sub

Private Sub UserForm_QueryClose(Cancel As Integer, CloseMode As Integer)
'Make sure all objects are destroyed
    If Not mcTree Is Nothing Then
        mcTree.TerminateTree
    End If
    Set mcTree = Nothing

End Sub

Private Function GetIcons(colImages As Collection, Optional ImageNames) As Long
    Dim v
    Dim img As MSForms.Image

    Set colImages = New Collection

    If IsMissing(ImageNames) Then
        ' get all available images
        For Each img In Me.frmImageBox.Controls
            colImages.add img.Picture, img.name
        Next
    Else
        ' only get specified images
        For Each v In ImageNames
            colImages.add Me.frmImageBox.Controls(v).Picture, v
        Next
    End If

    GetIcons = colImages.count

    ' all available images in frmImageBox on the demo form:
    '    ImageNames = Array("FLGBRAZL", "FLGCAN", "FLGFRAN", "FLGGERM", _
         "FLGNETH", "FLGSWED", "FLGUK", "FLGUSA02", _
         "NOTE03", "NOTE04", "OpenBook", _
         "FolderOpen", "FolderClosed", _
         "GreenTick", "Scroll", _
         "XPMinus", "XPPlus", _
         "Win7Minus","Win7Plus1","Win7Plus2")
End Function

Private Sub frTreeControl_Enter()
    If Not mcTree Is Nothing Then
        mcTree.EnterExit False
        If Not mcTree.ActiveNode Is Nothing Then
            mcTree_Click mcTree.ActiveNode
        End If
    End If
End Sub

Private Sub frTreeControl_Exit(ByVal Cancel As MSForms.ReturnBoolean)
    If Not mcTree Is Nothing Then
        mcTree.EnterExit True
'        TypeCombo.clear
'        ObjectCombo.clear
    End If
End Sub

Private Sub mcTree_AfterLabelEdit(Cancel As Boolean, NewString As String, cNode As clsNode)

' Validate user's manually edited node here
Dim i As Integer

'    For i = 1 To mcTree.jData.Children.count
'        If (mcTree.jData.Children(i).Value = NewString) Then
'            cNode.tag = mcTree.jData.Children(i).key
'            Exit For
'        End If
'    Next i
'    If (cNode.tag = "") Then
'        Exit Sub
'    End If

End Sub


'This gets fired when a node is clicked
Private Sub mcTree_Click(cNode As clsNode)
AppName = "mcTree_Click"
        On Error GoTo errH
    With cNode
        Me.labInfo.Caption = _
            "TreeName:  " & .TreeName & "   NodeId:  " & .NodeId & "    Pid:  " & .Pid & "  ObjectId:  " & .ObjectId & vbNewLine & _
            "ObjectTypeId:  " & .ObjectTypeId & "    TypeTable:  " & .TypeTable & "   RootPath:  " & .RootPath

    End With
    
    If Not ((TabTypes.tag = cNode.TreeName & "','" & cNode.RootPath) _
                Or (TabTypes.tag = cNode.TypeTable And Not (cNode.TypeTable = "") And cNode.TreeName = "")) Then
        If Not (cNode.TreeName = "") Then

            TabTypes.tag = cNode.TreeName & "','" & cNode.RootPath
            ControlFullFill "SELECT TYPE_TABLE FROM BRANCH_GET_TYPES('" & _
                         cNode.TreeName & "','" & cNode.RootPath & "') group by TYPE_TABLE", "TabTypes"
            TabTypes.tag = cNode.TreeName & "','" & cNode.RootPath
        Else
            TabTypes_Clear
        End If
        
        If Not (IsNull(cNode.TreeName) Or cNode.TreeName = "") Then
            If (TabTypes.NumTabs > 0) Then
                    GridUpdate TabTypes.TabData(TabTypes.CurrTab)
            Else
                GridUpdate
            End If
        Else
            GridUpdate cNode.TypeTable
        End If

    End If
Exit Sub
errH:
    If DebugMode = 1 Then
'        Stop
'        Resume
    End If
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
End Sub

'This gets fired when a key is pressed down
Private Sub mcTree_KeyDown(cNode As clsNode, ByVal KeyCode As MSForms.ReturnInteger, ByVal Shift As Integer)
' PT demo
    Dim bMove As Boolean
    Dim sMsg As String
    Dim cSource As clsNode
    
    AppName = "mcTree_KeyDown"
    On Error GoTo errH
    'If Not (frTreeControl.ActiveControl Is Nothing) Then
    Select Case KeyCode
    Case vbKeyUp, vbKeyDown, vbKeyLeft, vbKeyRight, _
         48 To 57, 96 To 105, vbKeyF2, 20, 93, _
         vbKeyPageUp, vbKeyPageDown, vbKeyHome, vbKeyEnd
        ' these keys are already trapped in clsTreeView for navigation, expand/collapse, edit mode

    Case vbKeyC, vbKeyC + 48
        If Shift = 2 Then    ' Ctrl-C copy
            ' code here to validate if user can copy this node
            Set mcTree.MoveCopyNode(False) = mcTree.ActiveNode
        End If
        
    Case vbKeyN, vbKeyN + 48
        If Shift = 2 Then    ' Ctrl-N copy
            'cmdAddChild_Click
        End If

    Case vbKeyX, vbKeyX + 48
        If Shift = 2 Then    ' Ctrl-X move
            ' code here to validate if user can move this node
            Set mcTree.MoveCopyNode(True) = mcTree.ActiveNode
        End If

    Case vbKeyV, vbKeyV + 48
        If Shift = 2 Then    ' Ctrl-V paste
            Set cSource = mcTree.MoveCopyNode(bMove)
            If Not cSource Is Nothing Then
                ' code to validate if the stored 'MoveCopyNode' can be Moved or Copied to the selected node
                If bMove Then
                    mcTree.Move cSource, mcTree.ActiveNode, bShowError:=True
                Else
                    mcTree.Copy cSource, mcTree.ActiveNode, bShowError:=True
                End If

                mcTree.ActiveNode.Sort    ' assume user wants move/copy to locate as sorted
                mcTree.ActiveNode.Expanded = True    ' assume user wants to see the moved/copied node if behind a collapsed node
                mcTree.refresh
            End If
        End If
    Case vbKeyDelete
            RemoveNode cNode
    Case vbKeyF5
        InitializeTrees
    End Select
'    End If
    Exit Sub
errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
    If Not mcTree Is Nothing Then
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
    End If
End Sub

Private Sub RemoveNode(cNode As clsNode)
    Dim sMsg As String
    Dim FBcon As ADODB.Connection
    Dim Fbcmd As ADODB.command
    Dim FbRst As ADODB.Recordset
    'Dim cParent As clsNode
    
    AppName = "RemoveNode"
    On Error GoTo errH
        'Set cParent = cNode.ParentNode
    
            sMsg = "Are you sure you want to delete node ''" & cNode.Caption & "'' and all it's child-nodes?" & vbCr & _
                        vbCr & "(press Ctrl-break now and click Debug to see this event code)"
        If MsgBox(sMsg, vbOKCancel, AppName) <> vbOK Then
            'mcTree.NodeRemove cNode
            'mcTree.refresh
            Exit Sub
        End If

    
    
        Call DB_CONNECT(FBcon, Fbcmd)
    Fbcmd.CommandText = "SELECT NODE_ID FROM TREE_BRANCH_DELETE('" & cNode.TreeName & "','" & _
        cNode.RootPath & "')"
    FBcon.BeginTrans
    Set FbRst = Fbcmd.execute
    
    If (Not FbRst.EOF) Then
        If (FbRst.Fields(0).Value = cNode.NodeId) Then
            FBcon.CommitTrans
            mcTree.NodeRemove cNode
            mcTree.refresh  ' refresh after all deleted nodes are removed
            mcTree_Click mcTree.ActiveNode
         End If
    Else
        MsgBox "TREE_BRANCH_DELETE error in " & Fbcmd.CommandText
    End If
FBcon.Close
Set FBcon = Nothing
    Exit Sub
errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
    If Not mcTree Is Nothing Then
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
    End If
End Sub

'Private Sub AddNode(ParentNode As clsNode, ObjectTypeId As Integer, ObjectId As Integer)
Private Sub AddNode(ParentNode As clsNode, TypeTable As String, ObjectId As Integer, Optional selNewNode As Boolean = True)
    Dim cNewNode As clsNode
    Dim cNode As clsNode
    Dim FBcon As ADODB.Connection
    Dim Fbcmd As ADODB.command
    Dim FbRst As ADODB.Recordset
    Dim ObjectTypeId As Integer
    Dim ObjectName As String
    Dim nKey As String
    Dim keyNum As Integer
    
    On Error GoTo errH
    AppName = "AddNode"
    keyNum = 0
'If (TypeCombo.Text <> "" And ObjectCombo.Text <> "") Then
    Call DB_CONNECT(FBcon, Fbcmd)
    Fbcmd.CommandText = "SELECT OBJECT_TYPE_ID, OBJECT_NAME FROM OBJECT_GET_TYPE('" & Replace(TypeTable, "'", "''") & "', " & ObjectId & ")"
    Set FbRst = Fbcmd.execute
    
    If (Not FbRst.EOF) Then
        ObjectTypeId = FbRst.Fields(0).Value
        ObjectName = FbRst.Fields(1).Value
    Else
        MsgBox "No result " & Fbcmd.CommandText
    Exit Sub
    End If
    Fbcmd.CommandText = "SELECT ID_NODE, OBJECT_ID, TYPE_ID, TYPE_TABLE " & _
                            "FROM TREE_NODE_APPEND('" & ParentNode.TreeName & _
        "', " & ObjectTypeId & " , " & ObjectId & ", '" & ParentNode.RootPath & "')"
    FBcon.BeginTrans
    Set FbRst = Fbcmd.execute
    
    If (Not FbRst.EOF) Then
    nKey = ParentNode.TreeName & "_" & FbRst.Fields(0).Value
    
                    For Each cNode In mcTree.Nodes
                        If cNode.key = nKey Then
                            nKey = nKey & "-" & nKey
                            keyNum = keyNum + 1
                            Exit For
                        End If
                    Next
                    
        Set cNewNode = ParentNode.AddChild(nKey, ObjectName)
    
                With cNewNode
                   .TreeName = ParentNode.TreeName
                   .NodeId = FbRst.Fields(0).Value
                   .Pid = IIf(ParentNode.NodeId = 0, Null, ParentNode.NodeId)
                   .ObjectId = FbRst.Fields(1).Value
                   .ObjectTypeId = FbRst.Fields(2).Value
                   .TypeTable = FbRst.Fields(3).Value
                   .RootPath = IIf(ParentNode.RootPath = "", "", ParentNode.RootPath & ",") & cNewNode.NodeId
                End With
        ParentNode.Expanded = True
        FBcon.CommitTrans
        mcTree.refresh    ' refresh the tree after adding all new nodes
        If selNewNode Then
            Set mcTree.ActiveNode = cNewNode ' could activate the new child
            mcTree.ScrollToView mcTree.ActiveNode, 2
            mcTree_Click mcTree.ActiveNode
        End If
    Else
        FBcon.RollbackTrans
        MsgBox "TREE_NODE_APPEND error in " & Fbcmd.CommandText
        ErrorLog "EMPTY RESULT error in QUERY: " & Fbcmd.CommandText, Err.Number, AppName
    End If
FBcon.Close
Set FBcon = Nothing
'End If



    Exit Sub
errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
    If Not mcTree Is Nothing Then
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
    End If
End Sub


Private Sub AddFromGrid(Row As Integer)
Dim i As Integer
Dim cNode As clsNode
    Set cNode = mcTree.ActiveNode
    If cNode Is Nothing Then Exit Sub
AppName = "AddFromGrid"
    On Error GoTo errH
If (left(TDBGrid1.columns(0).Caption, 3) = "ID_" Or Right(TDBGrid1.columns(0).Caption, 3) = "_ID" Or TDBGrid1.columns(0).Caption = "PKEY") Then
    'For i = 0 To ObjectCombo.ListCount - 1
    '    If CInt(ObjectCombo.list(i, 1)) = CInt(TDBGrid1.columns(0).CellValue(Row)) Then
    '        ObjectCombo.ListIndex = i
            'cmdAddChild_Click
    '        Exit Sub
    '    End If
    'Next i
    If TabTypes.NumTabs > 0 Then
    AddNode cNode, TabTypes.TabData(TabTypes.CurrTab), CInt(TDBGrid1.columns(0).CellValue(Row))
    Else
    MsgBox "No Type Tabs! Look branches!", , AppName
    End If
End If
    Exit Sub

errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
        ErrorDescription = Err.Description
        ErrorNumber = Err.Number
        MsgBox ErrorDescription, , AppName
        ErrorLog ErrorDescription, ErrorNumber, AppName
End Sub

Private Sub cmdAddAllFromGrid_Click()
Dim cNode As clsNode
Dim i As Integer
Set cNode = mcTree.ActiveNode
If TabTypes.NumTabs > 0 Then
    If (TDBGrid1.FirstRow <> "" And Not (TDBGrid1.EditActive) And TDBGrid1.SelBookmarks.count > 0) Then
        If (left(TDBGrid1.columns(0).Caption, 3) = "ID_" Or Right(TDBGrid1.columns(0).Caption, 3) = "_ID" Or TDBGrid1.columns(0).Caption = "PKEY") Then
            For i = 0 To TDBGrid1.SelBookmarks.count - 1
    
                AddNode cNode, TabTypes.TabData(TabTypes.CurrTab), _
                        CInt(TDBGrid1.columns(0).CellValue(TDBGrid1.SelBookmarks.item(i))), _
                        False
                        
        
        'AddFromGrid (TDBGrid1.FirstRow + TDBGrid1.SelBookmarks.item(i))
        'Set mcTree.ActiveNode = cNode ' could activate the new parent back
        'mcTree.ScrollToView mcTree.ActiveNode, 2
        'mcTree_Click mcTree.ActiveNode
            Next i
        End If
    End If
End If

End Sub

Private Sub cmdAddFromGrid_Click()
    TDBGrid1_DblClick
End Sub

Private Sub cmdProcExec_Click()
    Dim FBcon As ADODB.Connection
    Dim Fbcmd As ADODB.command
    Dim FbRst As ADODB.Recordset
    Dim i As Integer
    On Error GoTo errH
    AppName = "cmdProcExec_Click"
If (ProcCombo.Text <> "") Then
    Call DB_CONNECT(FBcon, Fbcmd)
    Fbcmd.CommandText = ProcCombo.Text
    Set FbRst = Fbcmd.execute
    If (InStr(1, UCase(ProcCombo.Text), "SELECT ", vbTextCompare) > 0) Then
        If Not FbRst.EOF Then
            FbRst.MoveFirst
            While Not FbRst.EOF
                For i = 0 To FbRst.Fields.count - 1
                    LblResult.Caption = LblResult.Caption & " " & CStr(FbRst.Fields(i).name) & "=" & CStr(FbRst.Fields(i).Value)
                Next i
                FbRst.MoveNext
            Wend
        End If
    Else
        LblResult.Caption = "OK"
    End If
FBcon.Close
Set FBcon = Nothing
End If
Exit Sub
errH:
    If DebugMode = 1 Then
        Stop
        Resume
    End If
    If Not mcTree Is Nothing Then
        MsgBox Err.Description, , AppName
        ErrorLog Err.Description, Err.Number, AppName
    End If

End Sub
