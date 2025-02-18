/**
 *    Copyright (C) 2015 MongoDB Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the GNU Affero General Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#pragma once

#include <vector>

#include "mongo/base/disallow_copying.h"
#include "mongo/base/status_with.h"
#include "mongo/bson/bsonobj.h"
#include "mongo/db/clientcursor.h"
#include "mongo/db/namespace_string.h"

namespace mongo {

/**
 * Builds a cursor response object from the provided cursor identifiers and "firstBatch",
 * and appends the response object to the provided builder under the field name "cursor".
 *
 * The response object has the following format:
 *   { id: <NumberLong>, ns: <String>, firstBatch: <Array> }.
 *
 * This function is deprecated.  Prefer CursorResponse::toBSON() instead.
 */
void appendCursorResponseObject(long long cursorId,
                                StringData cursorNamespace,
                                BSONArray firstBatch,
                                BSONObjBuilder* builder);

/**
 * Builds a getMore response object from the provided cursor identifiers and "nextBatch",
 * and appends the response object to the provided builder under the field name "cursor".
 *
 * The response object has the following format:
 *   { id: <NumberLong>, ns: <String>, nextBatch: <Array> }.
 *
 * This function is deprecated.  Prefer CursorResponse::toBSON() instead.
 */
void appendGetMoreResponseObject(long long cursorId,
                                 StringData cursorNamespace,
                                 BSONArray nextBatch,
                                 BSONObjBuilder* builder);

class CursorResponse {
    MONGO_DISALLOW_COPYING(CursorResponse);

public:
    enum class ResponseType {
        InitialResponse,
        SubsequentResponse,
    };

    /**
     * Constructs from values for each of the fields.
     */
    CursorResponse(NamespaceString nss,
                   CursorId cursorId,
                   std::vector<BSONObj> batch,
                   boost::optional<long long> numReturnedSoFar = boost::none);

#if defined(_MSC_VER) && _MSC_VER < 1900
    CursorResponse(CursorResponse&& other);
    CursorResponse& operator=(CursorResponse&& other);
#else
    CursorResponse(CursorResponse&& other) = default;
    CursorResponse& operator=(CursorResponse&& other) = default;
#endif

    //
    // Accessors.
    //

    const NamespaceString& getNSS() const {
        return _nss;
    }

    CursorId getCursorId() const {
        return _cursorId;
    }

    const std::vector<BSONObj>& getBatch() const {
        return _batch;
    }

    boost::optional<long long> getNumReturnedSoFar() const {
        return _numReturnedSoFar;
    }

    /**
     * Constructs a CursorResponse from the command BSON response.
     */
    static StatusWith<CursorResponse> parseFromBSON(const BSONObj& cmdResponse);

    /**
     * Converts this response to its raw BSON representation.
     */
    BSONObj toBSON(ResponseType responseType) const;
    void addToBSON(ResponseType responseType, BSONObjBuilder* builder) const;

private:
    NamespaceString _nss;
    CursorId _cursorId;
    std::vector<BSONObj> _batch;
    boost::optional<long long> _numReturnedSoFar;
};

}  // namespace mongo
