/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYINDEX_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYINDEX_H

#include <memory>
#include <boost/thread/mutex.hpp>
#include <db_cxx.h>
#include "../Index.h"
#include "../Transaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class KeyGenerator; 
	class Key;
	class Data;

	namespace BerkeleyDB {

		class BerkeleyObjectStore;

		///<summary>
		/// This class represents an index implementation backed by Berkeley DB.  This index type is 
		/// associated with a Berkeley DB database and automatically synchronizes keys between the two.
		///</summary>
		class BerkeleyIndex : public Index
			{
			public:
				BerkeleyIndex(BerkeleyObjectStore& objectStore, const std::string& name, const std::auto_ptr<KeyGenerator>& keyGenerator, const bool unique, TransactionContext& transactionContext, const bool create);
				virtual ~BerkeleyIndex(void);

				virtual Data get(const Key& secondaryKey, TransactionContext& transactionContext);
				virtual Key getPrimaryKey(const Key& secondaryKey, TransactionContext& transactionContext);
				virtual void put(const Key& secondaryKey, const Data& primaryKey, const bool noOverwrite, TransactionContext& transactionContext);
				virtual void remove(const Key& secondaryKey, TransactionContext& transactionContext);
				virtual void close();

			private:
				// The underlying index
				Db implementation;
				// Flag indicating whether the underlying index is still open
				volatile bool isOpen;
				// We'll need to synchronize some of our operations (e.g. closing)
				boost::mutex synchronization;

				// Berkeley DB-required method to automatically generate secondary keys
				static int callback(Db *secondary, const Dbt *key, const Dbt *data, Dbt *result);

				// The index cursor will need to access our implementation
				friend class BerkeleyIndexCursor;
			};
		}
	}
}
}

#endif