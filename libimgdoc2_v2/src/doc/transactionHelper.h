#pragma once

#include <functional>
#include <utility>

template <typename t_return_value>
class TransactionHelper
{
private:
    std::function< t_return_value()> action_;
    std::shared_ptr<IDbConnection> database_connection_;
public:
    TransactionHelper(
        const std::shared_ptr<IDbConnection>& database_connection,
        const std::function<t_return_value()>& action) :
        database_connection_(database_connection),
        action_(action)
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

            // TODO: I guess we need to think about how to deal with "execption from the next line"
            this->database_connection_->EndTransaction(true);

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
