#pragma once

#include <functional>
#include <utility>

/// A utility in order to wrap a piece of code into a database-transaction.
///
/// \tparam t_return_value  Type of the return value.
template <typename t_return_value>
class TransactionHelper
{
private:
    std::function< t_return_value()> action_;
    std::shared_ptr<IDbConnection> database_connection_;
public:
    TransactionHelper(
        std::shared_ptr<IDbConnection> database_connection,
        std::function<t_return_value()> action) :
        action_(std::move(action)),
        database_connection_(std::move(database_connection))
    {}

    t_return_value Execute()
    {
        bool transaction_initiated = false;
        if (!this->database_connection_->IsTransactionPending())
        {
            this->database_connection_->BeginTransaction();
            transaction_initiated = true;
        }

        try
        {
            t_return_value return_value = this->action_();

            if (transaction_initiated)
            {
                // TODO: I guess we need to think about how to deal with "exception from the next line"
                this->database_connection_->EndTransaction(true);
            }

            return return_value;
        }
        catch (...)
        {
            if (transaction_initiated)
            {
                this->database_connection_->EndTransaction(false);
            }

            throw;
        }
    }
};
