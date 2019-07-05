#if !defined(__CONFIG_DB_H)
#define __CONFIG_DB_H

#include "StdAfx.h"

class ConfigDb {
public:
    ConfigDb(std::string configFile);
    ~ConfigDb();

    rfa::config::ConfigDatabase& configDatabase() {
        return *_pConfigDatabase;
    }
    // Configuration tree that holds configuration
    const rfa::config::ConfigTree *getConfigTree( void) const;

    // Access to the configuration
    const std::string & getConfigurationClientName() const;
    const std::string & getConfigurationClientFullName() const;
    
    // Accessors to various configuration values
    long                    getLongConfigValue( const rfa::common::RFA_String & name, long defVal ) const;
    bool                    getBoolConfigValue( const rfa::common::RFA_String & name, bool defVal ) const;
    const    std::string     getStringConfigValue( const rfa::common::RFA_String & name, const rfa::common::RFA_String & defVal ) const;
    //StringList *            getStringListConfigValue(const std::string & name, 
    //                                                                       const char* dltr = DEFAULT_STR_DELIMITER) const;
    const rfa::config::ConfigTree *    getConfigSubTree( const rfa::common::RFA_String &name ) const;

    void                    printConfigDb(const rfa::common::RFA_String &fullname) const;
    rfa::common::RFA_String getConfigDb(const rfa::common::RFA_String &fullname) const;
    void                    setDebugMode(const bool &debug);
private:
    int loadStagingConfigDb();
    int populateConfigDb();    

    rfa::config::ConfigDatabase         *_pConfigDatabase;
    rfa::config::StagingConfigDatabase  *_pStagingDatabase;
    std::string                            _configFile;
    const rfa::config::ConfigTree*      _pConfigTree;
    std::string                            _name;
    std::string                            _fullName;
    bool                                _debug;
};    

#endif // !defined(__CONFIG_DB_H)
