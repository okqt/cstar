// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __HTMLDOC_HPP__
#define __HTMLDOC_HPP__

namespace HTML {
//! \brief Represents an HTML node.
class HTMLNode {
public:
  HTMLNode(const string& tag_, const string& id_, const string& data_) :
    mData(data_),
    mTag(tag_),
    mID(id_),
    mAutoID(""),
    mParent(0)
  {
    if (id_ != "") {
      char buf[256];
      sprintf(buf, "%s_%d", id_.c_str(), smAutoIDIndex++);
      mAutoID = buf;
      smAllNodes[id_] = this;
    } // if
  } // HTMLNode::HTMLNode
  HTMLNode(const string& tag_, const string& id_) :
    mData(""),
    mTag(tag_),
    mID(id_),
    mAutoID(""),
    mParent(0)
  {
    if (id_ != "") {
      char buf[256];
      sprintf(buf, "%s_%d", id_.c_str(), smAutoIDIndex++);
      mAutoID = buf;
      smAllNodes[id_] = this;
    } // if
  } // HTMLNode::HTMLNode
  virtual ~HTMLNode() {}
  virtual ostream& writeProperties(ostream& o) {
    /* LIE */
    return o;
  } // HTMLNode::writeProperties
  void Write(ostream& o) {
    o << "<" << mTag.c_str();
    if (mAutoID != "") {
      o << " id='" << mAutoID.c_str() << "'";
    } // if
    writeProperties(o) << ">\n" << mData.c_str();
    for (hFUInt32 c(0); c < mChildren.size(); ++ c) {
      mChildren[c]->Write(o);
    } // for
    o << "</" << mTag.c_str() << ">";
  } // HTMLNode::Write
  static void Add(const string& id_, HTMLNode* html_) {
    HTMLNode* lParent = smAllNodes[id_];
    lParent->mChildren.push_back(html_);
    html_->mParent = lParent;
  } // HTMLNode::Add
  static HTMLNode* Get(const string& id_) {
    return smAllNodes[id_];
  } // HTMLNode::Get
  string GetID() const {return mID;}
  string GetAutoID() const {return mAutoID;}
  string GetData() const {return mData;}
  hFUInt32 GetChildCount() const {return mChildren.size();}
private: // Member data
  string mData;
  string mTag;
  string mID;
  string mAutoID;
  HTMLNode* mParent;
  vector<HTMLNode*> mChildren;
  static map<string, HTMLNode*> smAllNodes;
  static hFInt32 smAutoIDIndex;
}; // class HTMLNode

class tagHTML : public HTMLNode {
public:
  tagHTML(const string& id_) : HTMLNode("html", id_) {}
}; // class tagHTML

class tagH1 : public HTMLNode {
public:
  tagH1(const string& id_, const string& data_) : HTMLNode("h1", id_, data_) {}
}; // class tagH1

class tagH2 : public HTMLNode {
public:
  tagH2(const string& id_, const string& data_) : HTMLNode("h2", id_, data_) {}
}; // class tagH2

class tagH3 : public HTMLNode {
public:
  tagH3(const string& id_, const string& data_) : HTMLNode("h3", id_, data_) {}
}; // class tagH3

class tagH4 : public HTMLNode {
public:
  tagH4(const string& id_, const string& data_) : HTMLNode("h4", id_, data_) {}
}; // class tagH4

class tagHR : public HTMLNode {
public:
  tagHR(const string& id_) : HTMLNode("hr", id_) {}
}; // class tagHR

class tagIMG : public HTMLNode {
public:
  tagIMG(const string& id_, const string& fileName_) : 
    HTMLNode("img", id_, ""), 
    mFileName(fileName_) 
  {
  } // tagIMG::tagIMG
  virtual ostream& writeProperties(ostream& o) {
    o << " src='" << mFileName.c_str() << "'";
    return o;
  } // HTMLNode::writeProperties
private:
  string mFileName;
}; // class tagIMG

class tagLINK : public HTMLNode {
public:
  tagLINK(const string& id_, HTMLNode* target_, const string& data_) : 
    HTMLNode("a", id_, data_), 
    mTarget(target_) 
  {
  } // tagLINK::tagLINK
  virtual ostream& writeProperties(ostream& o) {
    o << " href='#" << mTarget->GetAutoID().c_str() << "'";
    return o;
  } // HTMLNode::writeProperties
private:
  HTMLNode* mTarget;
}; // class tagLINK

class tagTITLE : public HTMLNode {
public:
  tagTITLE(const string& id_, const string& data_) : HTMLNode("title", id_, data_) {}
}; // class tagTITLE

class tagLISTITEM : public HTMLNode {
public:
  tagLISTITEM(const string& id_) : HTMLNode("li", id_) {}
}; // class tagLISTITEM

class tagLIST : public HTMLNode {
public:
  tagLIST(const string& id_, const string& data_) : HTMLNode("ul", id_, data_) {}
  tagLIST(const string& id_) : HTMLNode("ul", id_) {}
}; // class tagLIST

class tagBODY : public HTMLNode {
public:
  tagBODY(const string& id_) : HTMLNode("body", id_) {}
}; // class tagBODY

class tagHEADER : public HTMLNode {
public:
  tagHEADER(const string& id_) : HTMLNode("header", id_) {}
}; // class tagHEADER

}; // namespace HTML
using namespace HTML;

#endif

