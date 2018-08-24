#include "DapJsonParams.h"
#include "DapJsonCmd.h"

using Params = DapJsonParams::Params;

QMap<DapJsonCommands, QVector<Params>> DapJsonParams::availableParams {
    {DapJsonCommands::STATE, {DapJsonParams::STATE_NAME, DapJsonParams::VALUE}},

    {DapJsonCommands::CONNECTION, {Params::ADDRESS, Params::PORT,
                    Params::USER, Params::PASSWORD, Params::DISCONNECT}},

    {DapJsonCommands::STATS, {Params::READ_BYTES, Params::WRITE_BYTES}},

    {DapJsonCommands::GET_STATES, {}}, // GET_STATES Without params

    {DapJsonCommands::AUTHORIZE_ERROR, {Params::MESSAGE}}
};

bool DapJsonParams::isParamsAvailable(const DapJsonCommands& cmd, const Params& p) {
    if(!availableParams.contains(cmd)) {
        qWarning() << "Unknown DapJsonParams. Maybe add his to paramsName?";
        return false;
    }

    auto paramsList = availableParams.value(cmd);
    if(paramsList.isEmpty()) {
        qWarning() << "Something wrong params list for command:"
                   <<     DapJsonCmd::commandToString(cmd) << "is empty!";
        return false;
    }

    return paramsList.contains(p);
}

const QString& DapJsonParams::toString(Params p) {
    static QMap<Params, QString> paramsName = {
        {VALUE, "value"},
        {STATE_NAME, "state_name"},
        {READ_BYTES, "read_bytes"},
        {WRITE_BYTES, "write_bytes"},
        {ADDRESS, "address"},
        {PORT, "port"},
        {USER, "user"},
        {PASSWORD, "password"},
        {DISCONNECT, "disconnect"},
        {MESSAGE, "message"}
    };

    if (!paramsName.contains(p)) {
        throw std::runtime_error("Unknown DapJsonParams. Add his to paramsName!");
    }
    return paramsName[p];
}
