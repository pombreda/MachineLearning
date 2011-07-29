/** 
 @cond
 ############################################################################
 # LGPL License                                                             #
 #                                                                          #
 # This file is part of the Machine Learning Framework.                     #
 # Copyright (c) 2010, Philipp Kraus, <philipp.kraus@flashpixx.de>          #
 # This program is free software: you can redistribute it and/or modify     #
 # it under the terms of the GNU Lesser General Public License as           #
 # published by the Free Software Foundation, either version 3 of the       #
 # License, or (at your option) any later version.                          #
 #                                                                          #
 # This program is distributed in the hope that it will be useful,          #
 # but WITHOUT ANY WARRANTY; without even the implied warranty of           #
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
 # GNU Lesser General Public License for more details.                      #
 #                                                                          #
 # You should have received a copy of the GNU Lesser General Public License #
 # along with this program. If not, see <http://www.gnu.org/licenses/>.     #
 ############################################################################
 @endcond
 **/

#if defined(MACHINELEARNING_SOURCES) && defined(MACHINELEARNING_SOURCES_TWITTER)

#ifndef MACHINELEARNING_TOOLS_SOURCES_TWITTER_H
#define MACHINELEARNING_TOOLS_SOURCES_TWITTER_H

#include <ctime>
#include <limits>
#include <sstream>
#include <iostream>
#include <boost/asio.hpp>
#include <json/json.h>

#include "../../exception/exception.h"
#include "../language/language.h"
#include "../function.hpp"


namespace machinelearning { namespace tools { namespace sources {
    
    namespace bip  = boost::asio::ip;
    
    /** class for using Twitter
     * @see https://dev.twitter.com/docs
     * $LastChangedDate$
     **/
    class twitter {
        
        public :
        
            /** inner class for the twitter optional search parameters **/
            class searchparameter {
                
                public :
                
                    /** type of the resultset **/
                    enum resulttype {
                        mixed   = 0,
                        recent  = 1,
                        popular = 2
                    };
                
                    /** type of radius length **/
                    enum radiuslength {
                        kilometer = 0,
                        miles     = 1
                    };
                
                    /** struct of the geoposition **/
                    struct geoposition {
                        float latitude;
                        float longitude;
                        float radius;
                        radiuslength length;
                        
                        geoposition() : latitude(0), longitude(0), radius(0), length(kilometer) {}
                    };
                
                    searchparameter( void );
                    void setGeoPosition( const geoposition& );
                    void setUntilDate( const std::tm& );
                    void setLanguage( const language::code& );
                    void setResultType( const resulttype& );
                    void setResultCount( const std::size_t& );
                
                    friend std::ostream& operator<< ( std::ostream&, const searchparameter& );
        
                
                private :
                    
                    /** value witch data is used (binary coded) **/
                    std::size_t m_use;
                    
                    /** language code ISO 639-1 coded**/
                    language::code m_lang;
                    /** result type **/
                    std::string m_resulttype;
                    /** date value like YYYY-MM-DD for time-based searches **/
                    std::string m_until;
                    /** geoposition like latitude,longitude,radius_radiuslength **/
                    std::string m_geo;
                    /** number of results **/
                    std::size_t m_resultcount;
            };
        
        
            twitter( void );
            void setHTTPAgent( const std::string& );
            void search( const std::string& ); 
            void search( const std::string&, const searchparameter& );
            //void refresh( void );
            
            ~twitter( void );
        
        
        private :
        
            /** io service objekt for resolving the server name of the search server **/
            boost::asio::io_service m_iosearch;
            /** socket objekt for send / receive the data of search calls **/
            bip::tcp::socket m_socketsearch; 
            /** resolver of search connect **/
            bip::tcp::endpoint m_resolvesearch;
            /** name for the HTTP agent **/
            std::string m_httpagent;
            /** search parameter **/
            searchparameter m_searchparameter;
        
            std::string sendRequest( bip::tcp::socket&, const std::string&, const std::string& );
            void throwHTTPError( const unsigned int& ) const;   
            std::string urlencode( const std::string& ) const;
        
            void printValueTree( Json::Value&, const std::string& = "." ) const;
    };
    
    
    
    /** constructor **/
    inline twitter::twitter( void ) :
        m_iosearch(),
        m_socketsearch(m_iosearch),
        m_httpagent("Machine Learning Framework"),
        m_searchparameter()
    {
        boost::system::error_code l_error = boost::asio::error::host_not_found;

        // determine IP of the twitter seach server
        bip::tcp::resolver l_resolversearch(m_iosearch);
        bip::tcp::resolver::query l_searchquery("search.twitter.com", "http");
        
        for(bip::tcp::resolver::iterator l_endpoint = l_resolversearch.resolve( l_searchquery ); (l_error && l_endpoint != bip::tcp::resolver::iterator()); l_endpoint++) {
            m_socketsearch.close();
            m_resolvesearch = *l_endpoint;
            m_socketsearch.connect(*l_endpoint, l_error);
        }
        
        m_socketsearch.close();
        
        if (l_error)
            throw exception::runtime(_("can not connect to twitter search server"));
    }
    
    
    /** destructor **/
    inline twitter::~twitter( void )
    {
        m_socketsearch.close();
    }
    
    
    /** sets the name of the HTTP agent
     * @param p_agent name
     **/
    inline void twitter::setHTTPAgent( const std::string& p_agent) 
    {
        if (p_agent.empty())
            throw exception::runtime(_("HTTP agent name need not be empty"));
        m_httpagent = p_agent;
    }
    
    
    /** run a search with the last set of searchparameters / default parameters
     * @param p_search search string
     **/
    inline void twitter::search( const std::string& p_search )
    {
        search( p_search, m_searchparameter );
    }
    
    
    /** run search with search parameters
     * @param p_search search string
     * @param p_params search parameter
     **/
    inline void twitter::search( const std::string& p_search, const searchparameter& p_params )
    {
        if (p_search.empty())
            throw exception::runtime(_("search query need not be empty"));
        
        m_searchparameter = p_params;
        
        // create GET query (with default values)
        std::ostringstream l_query;
        l_query << "/search.json?q=" << function::urlencode(p_search) << "&" << p_params;
        
        // run request
        boost::system::error_code l_error = boost::asio::error::host_not_found;
        m_socketsearch.connect(m_resolvesearch, l_error);
        if (l_error)
            throw exception::runtime(_("can not connect to twitter search server"));        

        const std::string l_json = sendRequest( m_socketsearch, l_query.str(), "search.twitter.com" );
        m_socketsearch.close();
        
        if (l_json.empty())
            throw exception::runtime(_("no JSON data received"));
        
        // JSON parsing
        Json::Value l_twitterroot;
        //Json::Features l_jsonfeatures;
        Json::Reader l_jsonreader;
        
        if (!l_jsonreader.parse( l_json, l_twitterroot ))
            throw exception::runtime(_("JSON data can not be parsed"));
        
        printValueTree(l_twitterroot);
    }
    
    
    /** sends the HTTP request to the Twitter server and receives the header and returns the data
     * @param p_socket socket object
     * @param p_query query call
     * @param p_server server name
     * @return JSON answer data
     **/
    inline std::string twitter::sendRequest( bip::tcp::socket& p_socket, const std::string& p_query, const std::string& p_server )
    {
        // create HTTP request and send them over the socket        
        boost::asio::streambuf l_request;
        std::ostream l_request_stream(&l_request);
        l_request_stream << "GET " << p_query << " HTTP/1.1\r\n";
        l_request_stream << "Host: " << p_server << "\r\n";
        l_request_stream << "Accept: */*\r\n";
        l_request_stream << "User-Agent: " << m_httpagent << "\r\n";
        l_request_stream << "Connection: close\r\n\r\n";
        
        boost::asio::write( p_socket, l_request );
        
        // read first header line and extract HTTP status data
        boost::asio::streambuf l_response;
        boost::asio::read_until(p_socket, l_response, "\r\n");
        std::istream l_response_stream(&l_response);
        
        std::string l_http_version;
        unsigned int l_status;
        std::string l_status_message;
        
        l_response_stream >> l_http_version;
        l_response_stream >> l_status;
        
        std::getline(l_response_stream, l_status_message);
        if (!l_response_stream || l_http_version.substr(0, 5) != "HTTP/")
            throw exception::runtime(_("invalid response"));
        throwHTTPError( l_status );
        
        // read rest header until "double CR/LR"
        boost::asio::read_until(p_socket, l_response, "\r\n\r\n");
        // read each headerline, because on the socket can be more data than the header
        // so we read them until the header ends
        std::string l_header;
        while (std::getline(l_response_stream, l_header) && l_header != "\r");

        
        // read content data into a string stream
        std::ostringstream l_content( std::stringstream::binary );
        
        if (l_response.size() > 0)
            l_content << &l_response;
        
        boost::system::error_code l_error;
        while (boost::asio::read(p_socket, l_response, boost::asio::transfer_at_least(1), l_error));
        
        if (l_error != boost::asio::error::eof)
            throw exception::runtime(_("data can not be received"));

        l_content << &l_response;

        return l_content.str();
    }
    
    
    /** create an exception on the status code
     * @param p_status status code
     **/
    inline void twitter::throwHTTPError( const unsigned int& p_status ) const
    {
        switch (p_status) {
            case 0      : throw exception::runtime(_("error while reading socket data"));     
                
            case 203    : throw exception::runtime(_("Non-Authoritative Information"));
            case 204    : throw exception::runtime(_("No Content"));
            case 205    : throw exception::runtime(_("Reset Content"));
            case 206    : throw exception::runtime(_("Partial Content"));
            case 300    : throw exception::runtime(_("Multiple Choices"));
            case 301    : throw exception::runtime(_("Moved Permanently"));
            case 302    : throw exception::runtime(_("Moved Temporarily"));
            case 303    : throw exception::runtime(_("See Other"));
            case 304    : throw exception::runtime(_("Not Modified"));
            case 305    : throw exception::runtime(_("Use Proxy"));
            case 307    : throw exception::runtime(_("Temporary Redirect"));
            case 400    : throw exception::runtime(_("Bad Request"));
            case 401    : throw exception::runtime(_("Unauthorized"));
            case 402    : throw exception::runtime(_("Payment Required"));
            case 403    : throw exception::runtime(_("Forbidden"));
            case 404    : throw exception::runtime(_("Not Found"));
            case 405    : throw exception::runtime(_("Method Not Allowed"));
            case 406    : throw exception::runtime(_("Not Acceptable"));
                
            case 407    : throw exception::runtime(_("Proxy Authentication Required"));
            case 408    : throw exception::runtime(_("Request Time-out"));
            case 409    : throw exception::runtime(_("Conflict"));
            case 410    : throw exception::runtime(_("Gone"));
            case 411    : throw exception::runtime(_("Length Required"));
            case 412    : throw exception::runtime(_("Precondition Failed"));
            case 413    : throw exception::runtime(_("Request Entity Too Large"));
            case 414    : throw exception::runtime(_("Request-URI Too Large"));
            case 415    : throw exception::runtime(_("Unsupported Media Type"));
            case 416    : throw exception::runtime(_("Requested range not satisfiable"));
            case 417    : throw exception::runtime(_("Expectation Failed"));
            case 500    : throw exception::runtime(_("Internal Server Error"));
            case 501    : throw exception::runtime(_("Not Implemented"));
            case 502    : throw exception::runtime(_("Bad Gateway"));
            case 503    : throw exception::runtime(_("Service Unavailable"));
            case 504    : throw exception::runtime(_("Gateway Time-out"));
            case 505    : throw exception::runtime(_("HTTP Version not supported"));
        }
    }
    
    
    inline void twitter::printValueTree( Json::Value &value, const std::string &path ) const
    {
        switch ( value.type() ) {
            
            case Json::nullValue:
                printf("%s=null\n", path.c_str() );
                break;
            
            case Json::intValue:
                printf("%s=%d\n", path.c_str(), value.asInt() );
                break;
            
            case Json::uintValue:
                printf("%s=%u\n", path.c_str(), value.asUInt() );
                break;
            
            case Json::realValue:
                printf("%s=%.16g\n", path.c_str(), value.asDouble() );
                break;
            
            case Json::stringValue:
                printf("%s=\"%s\"\n", path.c_str(), value.asString().c_str() );
                break;
            
            case Json::booleanValue:
                printf("%s=%s\n", path.c_str(), value.asBool() ? "true" : "false" );
                break;
                
            case Json::arrayValue:
                printf("%s=[]\n", path.c_str() );
                for ( unsigned int index =0; index < value.size(); ++index )
                {
                    static char buffer[16];
                    sprintf( buffer, "[%d]", index );
                    printValueTree(value[index], path + buffer );
                }
                break;
                
            case Json::objectValue:
                printf("%s={}\n", path.c_str() );
                Json::Value::Members members( value.getMemberNames() );
                std::sort( members.begin(), members.end() );
                std::string suffix = *(path.end()-1) == '.' ? "" : ".";
                for ( Json::Value::Members::iterator it = members.begin(); 
                     it != members.end(); 
                     ++it )
                {
                    const std::string &name = *it;
                    printValueTree(value[name], path + suffix + name );
                }
                break;
        }
    }
    

    //======= Searchparameter ===========================================================================================================================
    
    
    /** constructor **/
    inline twitter::searchparameter::searchparameter( void ) :
        m_use(0),
        m_lang( language::EN ),
        m_resulttype(),
        m_until(),
        m_geo(),
        m_resultcount( 0 )
    {}
    
    /** set the search language
     * @param p_lang language option
     **/
    inline void twitter::searchparameter::setLanguage( const language::code& p_lang )
    {
        m_lang = p_lang;
        m_use  = m_use | 1;
    }
    
    /** set the result type
     * @param p_res result type
     **/
    inline void twitter::searchparameter::setResultType( const resulttype& p_res )
    {
        switch (p_res) {
            case mixed   :      m_resulttype = "mixed";   break;
            case popular :      m_resulttype = "popular"; break;
            case recent  :      m_resulttype = "recent";  break;
                
            default : throw exception::runtime(_("option value is unkown"));
        }
        m_use    = m_use | 2;
    }
    
    /** set the date value
     * @param p_time time struct
     **/
    inline void twitter::searchparameter::setUntilDate( const std::tm& p_time )
    {
        std::stringstream l_stream;
        
        l_stream << (p_time.tm_year+1900) << "-" << std::setfill('0');
        
        l_stream << std::setw(2) << p_time.tm_mon;
        l_stream << std::setw(0) << "-";
        l_stream << std::setw(2) << p_time.tm_mday;
        
        m_until = l_stream.str();
        m_use   = m_use | 4;
    }
    
    /** set the geo position
     * @param p_geo geostruct
     **/
    inline void twitter::searchparameter::setGeoPosition( const geoposition& p_geo )
    {
        std::stringstream l_stream;
        
        l_stream << p_geo.latitude << ",";
        l_stream << p_geo.longitude << ",";
        l_stream << std::abs(p_geo.radius);
        switch (p_geo.length) {
            case kilometer :    l_stream << "km"; break;
            case miles     :    l_stream << "mi"; break;
        }
        
        m_geo = l_stream.str();
        m_use = m_use | 8;
    }

    
    /** sets the number of results
     * @param p_num number
     **/
    inline void twitter::searchparameter::setResultCount( const std::size_t& p_num )
    {
        if ((p_num == 0) || (p_num > 1500))
            throw exception::runtime(_("result number must be in the range [1,1500]"));
            
        m_resultcount = p_num;
        m_use         = m_use | 16;
    }
    
    
    /** overloaded << operator for creating the string representation of the object
     * @param p_stream output stream
     * @param p_obj object
     * @return result stream with added data
     **/
    inline std::ostream& operator<< ( std::ostream& p_stream, const twitter::searchparameter& p_obj )
    {
        if (p_obj.m_use & 1)
            p_stream << "lang=" << language::toString(p_obj.m_lang) << "&";
         
        if (p_obj.m_use & 2)
            p_stream << "result_type=" << p_obj.m_resulttype << "&";
         
        if (p_obj.m_use & 4)
            p_stream << "until=" << p_obj.m_until << "&";
         
        if (p_obj.m_use & 8)
            p_stream << "geocode=" << p_obj.m_geo << "&";
        
        if (p_obj.m_use & 16) //max 15 pages (page), max 100 per page (rpp)
            p_stream << "rpp=" << p_obj.m_resultcount << "&";
         
        return p_stream;
    }

    
};};};



#endif
#endif