/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_LIFECYCLEOBSERVABLE_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_LIFECYCLEOBSERVABLE_H

#include <list>
#include <boost/enable_shared_from_this.hpp>
#include "LifeCycleObserver.h"
#include "Container.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class Transaction;
typedef boost::shared_ptr<Transaction> TransactionPtr;

namespace Support {

///<summary>
/// This class represents an entity that implements the observer pattern; other classes may observe
/// it to determine if it contains a transaction that has been committed or aborted.  Observers
/// are also notified when this object is closed.
/// TODO: Pretty sure we can refactor out the transactional methods here; there's no need to chain these
///</summary>
template<class T>
class LifeCycleObservable : public boost::enable_shared_from_this<LifeCycleObservable<T> >
	{
	public:
        typedef boost::shared_ptr<LifeCycleObservable<T> > LifeCycleObservablePtr;
        typedef boost::shared_ptr<LifeCycleObserver<T> > LifeCycleObserverPtr;
        typedef boost::weak_ptr<LifeCycleObserver<T> > LifeCycleObserverWeakPtr;
		// Add a new observer to this observable class
		void addLifeCycleObserver(const LifeCycleObserverPtr& observer)
			{ observers.push_back(observer); }

        struct removeWeakPtr : public std::unary_function<boost::weak_ptr<T>, bool>
        {
            removeWeakPtr(const LifeCycleObserverWeakPtr& ptr) : inner(ptr) { }

            LifeCycleObserverWeakPtr inner;
            bool operator()(const LifeCycleObserverWeakPtr& b) {
                return inner.lock() == b.lock();
            }
        };

        virtual boost::shared_ptr<T> get_object() { return FB::ptr_cast<T>(shared_from_this()); }

		// Remove an observer from this observable class
		void removeLifeCycleObserver(const LifeCycleObserverPtr& observer)
			{ observers.remove_if(removeWeakPtr(observer));}

	public:
		// These methods are executed when the derived class wishes to fire the transaction event
		virtual void onTransactionCommitted(const TransactionPtr& transaction) = 0;
		virtual void onTransactionAborted(const TransactionPtr& transaction) = 0;

		// This method is called when the derived class is about to close; it notifies all 
		// observers of this fact
		void raiseOnCloseEvent()
			{ 
			for_each(observers.begin(), observers.end(), ExecuteCloseFunctor(FB::ptr_cast<T>(shared_from_this()))); 
			observers.clear();
			}

	private:
		// A list of our current observers
		std::list<LifeCycleObserverWeakPtr> observers;

		// Functor that calls the onClose event on a given observer
		struct ExecuteCloseFunctor
			{
			const boost::shared_ptr<T> observable;
			ExecuteCloseFunctor(const boost::shared_ptr<T>& observable) 
				: observable(observable) { }

			void operator ()(const LifeCycleObserverWeakPtr& observer) 
			    {
                LifeCycleObserverPtr p(observer.lock());
                if (p)
                    {
                    p->onClose(observable);
                    }
                }
			};

		friend class Support::Container<T>;
	};

}
}
}
}

#endif