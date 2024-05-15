package core

import (
	"fmt"
	"strings"
)

const qOpSelect = "SELECT"
const qOpUpdate = "UPDATE"

type selectQuery struct {
    columns []string
    limit uint
}

type updateQuery struct {
    columns []string
    columnValues []interface{}
}

type QueryBuilder struct {
    operation string
    tableName string

    conditions []string
    conditionValues []interface{}
    
    qSelect selectQuery
    qUpdate updateQuery
}

func Select(columns ...string) *QueryBuilder {
    return &QueryBuilder{
        operation: qOpSelect,
        tableName: "",
        conditions: make([]string, 0),
        conditionValues: make([]interface{}, 0),

        qSelect : selectQuery {
            columns: columns,
            limit: 1,
        },
    }
}

func Update(table string) *QueryBuilder {
    return &QueryBuilder{
        operation: qOpUpdate,
        tableName: table,
        conditions: make([]string, 0),
        conditionValues: make([]interface{}, 0),
        qUpdate : updateQuery {
            columns: make([]string, 0),
            columnValues: make([]interface{}, 0),
        },
    }

}

func (q *QueryBuilder) Table(name string) *QueryBuilder {
    q.tableName = name
    return q
}

func (q *QueryBuilder) Where(column string, operator string, value interface{}) *QueryBuilder {
    q.conditions = append(q.conditions, fmt.Sprintf("%s %s ?", column, operator))
    q.conditionValues = append(q.conditionValues, value)
    return q
}

func (q *QueryBuilder) Limit(amount uint) *QueryBuilder {
    if q.operation == qOpSelect {
        q.qSelect.limit = amount
    }
    return q
}

func (q *QueryBuilder) toSelectQueryString() string {
    var columnsString = "*"
    var conditionsString = ""
    if len(q.qSelect.columns) != 0 {
        columnsString = strings.Join(q.qSelect.columns, ",")
    }

    if len(q.conditions) != 0 {
        conditionsString = fmt.Sprintf("WHERE %s", strings.Join(q.conditions, ","))
    }

    if q.qSelect.limit != 0 {
        return fmt.Sprintf("SELECT %s FROM %s %s LIMIT %d", columnsString, q.tableName, conditionsString, q.qSelect.limit)
    } else {
        return fmt.Sprintf("SELECT %s FROM %s %s", columnsString, q.tableName, conditionsString)
    }
}

func (q *QueryBuilder) Set(column string, value interface{}) *QueryBuilder {
    q.qUpdate.columns = append(q.qUpdate.columns, fmt.Sprintf("%s=?", column))
    q.qUpdate.columnValues = append(q.qUpdate.columnValues, value) 
    return q
}

func (q *QueryBuilder) toUpdateQueryString() string {
    sets := strings.Join(q.qUpdate.columns, ",")
    if len(q.conditions) > 0 {
        conditions := strings.Join(q.conditions, ",")
        return fmt.Sprintf("UPDATE %s SET %s WHERE %s", q.tableName, sets, conditions)
    } else {
        return fmt.Sprintf("UPDATE %s SET %s", q.tableName, sets)
    }
}

func (q *QueryBuilder) ToString() string {
    if q.operation == qOpSelect {
        return q.toSelectQueryString()
    } else if q.operation == qOpUpdate {
        return q.toUpdateQueryString()
    } else {
        return ""
    }
}

func (q *QueryBuilder) Values() []interface{} {
    values := make([]interface{}, 0)
    if q.operation == qOpUpdate {
        values = append(values, q.qUpdate.columnValues...)
    }
    values = append(values, q.conditionValues...)
    return values
}
