/*
 * MpdXMLNode.cxx
 *
 *  Created on: 01-10-2013
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */

#include "MpdXMLNode.h"
#include "TDOMParser.h"
#include "TXMLEngine.h"
#include <iostream>


MpdXMLNode::MpdXMLNode(TString name,TString value) : TNamed(name, value){
	fChildren.SetOwner(kTRUE);
	fAttrib.SetOwner(kTRUE);
}

MpdXMLNode::MpdXMLNode(const MpdXMLNode& other):
        MpdXMLNode(other.GetName(),other.GetValue()){
	for(int i=0;i<other.fChildren.GetEntries();i++){
        fChildren.Add(new MpdXMLNode(*other.GetChild(i)));
	}
	for(int i=0;i<other.fAttrib.GetEntries();i++){
        fAttrib.Add(new MpdXMLAttrib(*other.GetAttrib(i)));
	}
}

MpdXMLNode& MpdXMLNode::operator=(const MpdXMLNode& other) {
	if(&other == this)
	   return *this;
	SetName(other.GetName());
	SetValue(other.GetValue());
	fChildren.Clear();
	fAttrib.Clear();
	for(int i=0;i<other.fChildren.GetEntries();i++){
        fChildren.Add(new MpdXMLNode(*other.GetChild(i)));
	}
	for(int i=0;i<other.fAttrib.GetEntries();i++){
        fAttrib.Add(new MpdXMLAttrib(*other.GetAttrib(i)));
	}
	return *this;
}

void MpdXMLNode::Copy(TXMLNode* node){
	fChildren.Clear();
	fAttrib.Clear();
	SetName(node->GetNodeName());
	SetTitle(node->GetText());
	if(node->HasChildren()){
		TXMLNode *child = node->GetChildren();
		do{
			if(child==NULL) break;
			TString name = child->GetNodeName();
			if(name!="text"){//skip "text" nodes
                MpdXMLNode *tempnode = new MpdXMLNode();
				tempnode->Copy(child);
				fChildren.Add(tempnode);
			}
			if(child->HasNextNode())
				child=child->GetNextNode();
		}while(child->HasNextNode());
	}
	if(node->HasAttributes()){
		TList *atr_list = node->GetAttributes();
		for(int i=0;i<atr_list->GetEntries();i++){
			TXMLAttr *atrib = (TXMLAttr*)atr_list->At(i);
            fAttrib.Add(new MpdXMLAttrib(atrib->GetName(),atrib->GetValue()));
		}
	}
}

void MpdXMLNode::AddAttrib(MpdXMLAttrib* attrib) {
	TString new_atr = attrib->GetName();
	if(GetAttrib(new_atr)!=nullptr){
        std::cout<<"MpdXMLNode::AddAttrib Can't have two attributes with the same name!"<<std::endl;
		return;
	}
	fAttrib.AddLast(attrib);
}

Int_t MpdXMLNode::GetNChildren(TString name) const{
	Int_t counter =0;
	for(int i=0;i<GetNChildren();i++){
		TString name_temp = GetChild(i)->GetName();
		if(name_temp==name){
			counter++;
		}
	}
	return counter;
}

MpdXMLNode* MpdXMLNode::GetChild(TString name, Int_t count) const{
	Int_t control_index = 0;
	for(int i=0;i<fChildren.GetEntries();i++){
        MpdXMLNode *node = GetChild(i);
		TString temp = node->GetName();
		if(temp==name){
			control_index++;
		}
		if(control_index>count)
			return node;
	}
	return nullptr;
}

MpdXMLAttrib* MpdXMLNode::GetAttrib(TString name) const{
    return static_cast<MpdXMLAttrib*>(fAttrib.FindObject(name));
}

MpdXMLNode* MpdXMLNode::GetChild(Int_t index) const{
    return static_cast<MpdXMLNode*>(fChildren.At(index));
}

MpdXMLAttrib* MpdXMLNode::GetAttrib(Int_t index) const{
    return static_cast<MpdXMLAttrib*>(fAttrib.At(index));
}

MpdXMLNode::~MpdXMLNode() {
}

//---------- MpdXMLFile ------------------------------------------------------------------------------------

MpdXMLFile::MpdXMLFile(TString name, TString mode) :fName(name){
	if(mode=="read"||mode=="READ"){
		fOverwrite  = kFALSE;
		TDOMParser Parser;
		Parser.SetValidate(kFALSE);
		Parser.ParseFile(name);
		TXMLNode *MainNode = Parser.GetXMLDocument()->GetRootNode();
        fRootNode.reset(new MpdXMLNode());
		fRootNode->Copy(MainNode);
	}else{
		fOverwrite  = kTRUE;
	}
}

void MpdXMLFile::CreateRootNode(TString name) {
    fRootNode.reset(new MpdXMLNode(name));
}

void MpdXMLFile::SetRootNode(MpdXMLNode *node) {
	fRootNode.reset(node);
}

void MpdXMLFile::Close() {
	if(fOverwrite){
		if(!fRootNode){
            std::cout<<"MpdXMLFile::Close() No root node!"<<std::endl;
			return;
		}
		TXMLEngine engine;
		XMLNodePointer_t mainnode = engine.NewChild(0, 0, fRootNode->GetName());
		ExportNode(mainnode,engine,*fRootNode.get());
		XMLDocPointer_t xmldoc =  engine.NewDoc();
		engine.DocSetRootElement(xmldoc,mainnode);
		engine.SaveDoc(xmldoc,fName);
		engine.FreeDoc(xmldoc);
		fRootNode.reset(nullptr);
	}
}

void MpdXMLFile::ExportNode(XMLNodePointer_t& nodePointer,
        TXMLEngine &engine,const MpdXMLNode &node) const {
	for(int i=0;i<node.GetNChildren();i++){
		XMLNodePointer_t child = engine.NewChild(nodePointer,0,node.GetChild(i)->GetName(),node.GetChild(i)->GetValue());
		for(int j=0;j<node.GetChild(i)->GetNAttributes();j++){
			engine.NewAttr(child,0,node.GetChild(i)->GetAttrib(j)->GetName(),node.GetChild(i)->GetAttrib(j)->GetValue());
		}
		ExportNode(child,engine,*node.GetChild(i));
	}
}

MpdXMLFile::~MpdXMLFile() {
	if(fRootNode&&fOverwrite)
		Close();
}
