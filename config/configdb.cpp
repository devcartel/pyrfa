//
//|---------------------------------------------------------------
//|                Copyright (C) 2003 Reuters,                  --
//|         3 Times Square, New York, NY 10036                  --
//| All rights reserved. Duplication or distribution prohibited --
//|---------------------------------------------------------------

/*
 *  This configDb creates and loads the configDatabase 
 *  as part of the constructor.  Later we will refine 
 *  this utility class to allow more control.
 */

#include "config/configdb.h"
#include "Config/ConfigDatabase.h"
#include "Config/ConfigTree.h"
#include "Config/ConfigNode.h"
#include "Config/ConfigString.h"
#include "Config/ConfigLong.h"
#include "Config/ConfigBool.h"
#include "Common/Exception.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace rfa::common;
using namespace rfa::config;

extern "C" void checkException(rfa::common::Exception& e);

// ---------------------------------------------
ConfigDb::ConfigDb(std::string configFile) :
    _pConfigDatabase(0)
    ,_pStagingDatabase(0)
    ,_configFile(configFile)
    ,_pConfigTree(0)
    ,_name("")
    ,_fullName("")
    ,_debug(false)
// ---------------------------------------------
{
    if (populateConfigDb() != 0) {
        exit(-6);
    }
    /*
    const rfa::config::ConfigTree* root = _pConfigDatabase->getConfigTree();
    if (root == NULL) {
        cout << "Database has no root" << endl;
        exit(-3);
    }
    const string rootnodename = 
        ((root->getNodename().length() == 0) ? "No node name" : root->getNodename());
    const string roottype = 
        ((root->getTypeAsString().length() == 0) ? "No node name" : root->getTypeAsString());
    const string rootfullname = 
        ((root->getFullName().length() == 0) ? "No node name" : root->getFullName());

    _name = rootnodename;
    _fullName = rootfullname;
    _pConfigTree = root;
    */
}

/*
* printConfigDb
*
* To recursively print out configuration parameters to screen.
* fullname is passed like "\\Default\\Sessions"
*
*/
void ConfigDb::printConfigDb(const rfa::common::RFA_String &fullname) const {       
    const rfa::config::ConfigTree *pRootConfigTree = _pConfigDatabase->getConfigTree();
    const rfa::config::ConfigNode *pConfigNode = pRootConfigTree->getNode( fullname );
    if(!pConfigNode) {
        return;
    }
    const rfa::config::ConfigTree *pConfigTree = static_cast<const ConfigTree*>(pConfigNode);
    ConfigNodeIterator *pIt = pConfigTree->createIterator();
    assert(pIt);
    pConfigNode = 0;
    for (pIt->start();!pIt->off();pIt->forth()) {
        pConfigNode = pIt->value();
        //std::cout << "FullName:" << (pConfigNode->getFullName()).c_str() << std::endl;
        switch(pConfigNode->getType()) {
            case treeNode: {
                printConfigDb(pConfigNode->getFullName());
                break;
            }
            case stringValueNode:
            case wideStringValueNode: {
                const ConfigString * configParam = static_cast<const rfa::config::ConfigString*>(pConfigNode);
                cout << pConfigNode->getFullName().c_str();
                cout << " = ";
                cout << configParam->getValue().c_str() << std::endl;
                break;
            }
            case longValueNode: {
                const ConfigLong * configParam = static_cast<const rfa::config::ConfigLong*>(pConfigNode);
                cout << pConfigNode->getFullName().c_str();
                cout << " = ";
                cout << configParam->getValue() << std::endl;
                break;
            }
            case boolValueNode: {
                const ConfigBool * configParam = static_cast<const rfa::config::ConfigBool*>(pConfigNode);
                cout << pConfigNode->getFullName().c_str();
                cout << " = ";
                if(configParam->getValue()) {
                    cout << "true" << std::endl;
                } else {
                    cout << "false" << std::endl;
                }
                break;
            }
            default:  {
                cout << "Unknown ConfigNode Type for " << pConfigNode->getFullName().c_str() << endl;
            }
        }
    }
    pIt->destroy();
}

/*
* getConfigDb
*
* Return an exact parameter that matches fullname as RFA_String
* fullname is passed like "\\Default\\Connections\\Connection_RSSL\\UserName"
* return empty string if not found.
*
*/
rfa::common::RFA_String ConfigDb::getConfigDb(const rfa::common::RFA_String &fullname) const {
    const rfa::config::ConfigTree *pRootConfigTree = _pConfigDatabase->getConfigTree();
    const ConfigNode *pConfigNode = pRootConfigTree->getNode( fullname );;
    if(!pConfigNode) {
        if(_debug)
            cerr << "[ConfigDb::getConfigDb] Retrieving a param from config db failed for: " << fullname.c_str() << endl << flush;
        return rfa::common::RFA_String("");
    }
    assert(pConfigNode);
    switch(pConfigNode->getType()) {
        case treeNode: {
            if(_debug)
                cerr << "Not a full qualified name: " << fullname.c_str() << endl;
            return rfa::common::RFA_String("");
        }
        case longValueNode: {
            const ConfigLong * configParam = static_cast<const rfa::config::ConfigLong*>(pConfigNode);
            if(_debug) {
                cout << "[ConfigDb::getConfigDb] " << pConfigNode->getFullName().c_str();
                cout << " = ";
                cout << configParam->getValue() << endl;
            }
            return rfa::common::RFA_String(boost::lexical_cast<std::string>(configParam->getValue()).c_str());
        }
        case stringValueNode:
        case wideStringValueNode: {
            const ConfigString * configParam = static_cast<const rfa::config::ConfigString*>(pConfigNode);
            if(_debug) {
                cout << "[ConfigDb::getConfigDb] " << pConfigNode->getFullName().c_str();
                cout << " = ";
                cout << configParam->getValue().c_str() << endl;
            }
            return configParam->getValue();
        }
        case boolValueNode: {
            const ConfigBool * configParamDisplay = static_cast<const rfa::config::ConfigBool*>(pConfigNode);
            if(_debug) {
                cout << "[ConfigDb::getConfigDb] " << pConfigNode->getFullName().c_str();
                cout << " = ";
            }
            if(configParamDisplay->getValue()) {
                if(_debug)
                    cout << "true" << std::endl;
                return rfa::common::RFA_String("true");
            } else {
                if(_debug)
                    cout << "false" << std::endl;
                return rfa::common::RFA_String("false");
            }
        }
        default:  {
            cout << "Unknown ConfigNode Type for " << pConfigNode->getFullName().c_str() << endl;
            return rfa::common::RFA_String("");
        }
    }
}

// ---------------------------------------------
ConfigDb::~ConfigDb() {
// ---------------------------------------------
    // Release Configuration Database
    if (_pStagingDatabase)                _pStagingDatabase->destroy();
    if (_pConfigDatabase)                _pConfigDatabase->release();
}

// ---------------------------------------------
int ConfigDb::loadStagingConfigDb() {
// ---------------------------------------------
    try {
        if (_configFile == "") {
            std::cout << "Loading RFA config from registry "
                << std::endl;
            return (!_pStagingDatabase->load(rfa::config::windowsRegistry,
                    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Reuters\\RFA\\Default"));
        } else {
            //std::cout << "Loading RFA config from file '" << _configFile
            //    << "'" << std::endl;
            return (!_pStagingDatabase->load(rfa::config::flatFile,
                    _configFile.c_str()));
        }
    } catch (rfa::common::Exception& e)     {
        cerr << "Load of config db failed '" << _configFile << "'" << endl << flush;
        checkException(e);
    }
    return 1;
}


// -----------------------------------------
int ConfigDb::populateConfigDb() {
// -----------------------------------------
    // Populate the ConfigDatabase
    _pConfigDatabase = 
        rfa::config::ConfigDatabase::acquire("RFA");
    _ASSERTE (_pConfigDatabase);

    _pStagingDatabase = 
        rfa::config::StagingConfigDatabase::create();
    _ASSERTE (_pStagingDatabase);

    if (loadStagingConfigDb() != 0) { 
        // **** error loading staging db
        _pStagingDatabase->destroy();
        _pConfigDatabase->release();
        return -1;
    } else {
        // **** merge the staging into the database
        if (!_pConfigDatabase->merge(*_pStagingDatabase)) {
            _pStagingDatabase->destroy();
            _pConfigDatabase->release();
            return -1 ;
        }
        
    }
    return 0;
}
// --------------------------------------------------------
const std::string & ConfigDb::getConfigurationClientName() const
// --------------------------------------------------------
{
    return _name;
}
        
// ------------------------------------------------------------
const std::string & ConfigDb::getConfigurationClientFullName() const
// ------------------------------------------------------------
{
    return _fullName;
}

// -----------------------------------------------------
const rfa::config::ConfigTree * ConfigDb::getConfigTree( void) const
// -----------------------------------------------------
{
    /*
    if( !_pConfigTree ) {
        // Get component root
        assert( _pConfigDatabase );
        const rfa::config::ConfigTree *pRoot = 
            _pConfigDatabase->getConfigTree();
        assert( pRoot );
        if (pRoot == 0) {
            cout << "Configuration Error: Cannot retrieve configuration from registry." << endl;
            exit(0);
        } else {
            const rfa::config::ConfigNode *pNode = 
                pRoot->getNode( _fullName );

            if( pNode && pNode->getType() == treeNode ) {
                _pConfigTree = static_cast<const ConfigTree *>(pNode);
                assert( _pConfigTree );
            }
        }
    }
    */

    // Note: 0 will be returned if there's no config available
    return _pConfigTree;
}

// --------------------------------------------------------------------------
long ConfigDb::getLongConfigValue( const rfa::common::RFA_String & name, long defVal ) 
    const
// -------------------------------------------------------------------------- const
{
    const rfa::config::ConfigNode *pNode = 0; 

    if( getConfigTree() 
        && ( pNode = getConfigTree()->getNode( name ) ) != 0
        && pNode->getType() == longValueNode )
    {
        //const ConfigLong *pLongVal = static_cast< const ConfigLong *>( pNode );
        //assert( pLongVal );
    
        return ((ConfigTree*)pNode)->getChildAsLong(name, defVal);
    }
    else
        return  defVal;
}

// --------------------------------------------------------------------------
bool ConfigDb::getBoolConfigValue( const rfa::common::RFA_String & name, bool defVal ) const
 // --------------------------------------------------------------------------
{
    const rfa::config::ConfigNode *pNode = 0;

    if( getConfigTree() 
        && ( pNode = getConfigTree()->getNode( name ) ) != 0
        && pNode->getType() == boolValueNode )
    {
        //const rfa::config::ConfigBool *pBoolVal = 
        //    static_cast< const ConfigBool *>( pNode );
        //assert( pBoolVal );
    
        return ((ConfigTree*)pNode)->getChildAsBool(name, defVal);
    }
    else
        return  defVal;
}

// --------------------------------------------------------------------------
const std::string ConfigDb::getStringConfigValue( const rfa::common::RFA_String &name
                                        , const rfa::common::RFA_String &defVal ) const
// --------------------------------------------------------------------------
{
    const rfa::config::ConfigNode *pNode = 0; 

    if( getConfigTree() 
        && ( pNode = getConfigTree()->getNode( name ) ) != 0
        && pNode->getType() == stringValueNode )
    {
        //const ConfigString *pStringVal = static_cast< const ConfigString *>( pNode );
        //assert( pStringVal );
    
        return ((ConfigTree*)pNode)->getChildAsString(name, defVal).c_str();
    }
    else
        return  defVal.c_str();
}
/*
// --------------------------------------------------------------------------
StringList *  ConfigDb::getStringListConfigValue(const std::string & name
                                                 ,const char* dltr) const
// --------------------------------------------------------------------------
{
    const ConfigNode *pNode = 0;

    if( getConfigTree() 
        && ( pNode = getConfigTree()->getNode( name ) )  != 0
        && pNode->getType() == stringListValueNode )
    {
        //const ConfigStringList *pStringVal = static_cast< const ConfigStringList *>( pNode );
        //assert( pStringVal );
    
        return ((ConfigTree*)pNode)->getChildAsStringList(name, "", dltr);
    }
    else
        return  0;
}
*/
// ------------------------------------------------------------------------
const rfa::config::ConfigTree * ConfigDb::getConfigSubTree( const rfa::common::RFA_String &name ) 
                                                        const
// ------------------------------------------------------------------------
{
    const rfa::config::ConfigNode *pNode = 0;

    if( getConfigTree() 
        && ( pNode = getConfigTree()->getNode( name ) ) != 0
        && pNode->getType() == treeNode )
    {
        const ConfigTree *pConfigTree = static_cast<const ConfigTree *>(pNode);
        assert( pConfigTree );
        return pConfigTree;
    }
    else
        return 0;
}

void ConfigDb::setDebugMode(const bool &debug) {
    _debug = debug;
}
