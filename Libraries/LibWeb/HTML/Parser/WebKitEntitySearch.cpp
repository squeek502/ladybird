/*
 * Copyright (C) 2023 Apple, Inc. All rights reserved.
 * Copyright (C) 2010-2014 Google, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include <AK/StringView.h>
#include <LibWeb/HTML/Parser/WebKitEntitySearch.h>

namespace Web::HTML {

WebKitHTMLEntitySearch::WebKitHTMLEntitySearch()
    : m_entries(WebKitHTMLEntityTable::entries())
{
}

WebKitHTMLEntitySearch::CompareResult WebKitHTMLEntitySearch::compare(const WebKitHTMLEntityTableEntry* entry, u16 nextCharacter) const
{
    u16 entryNextCharacter;
    if (entry->nameLengthExcludingSemicolon < m_currentLength + 1) {
        if (!entry->nameIncludesTrailingSemicolon || entry->nameLengthExcludingSemicolon < m_currentLength)
            return Before;
        entryNextCharacter = ';';
    } else
        entryNextCharacter = entry->nameCharacters()[m_currentLength];
    if (entryNextCharacter == nextCharacter)
        return Prefix;
    return entryNextCharacter < nextCharacter ? Before : After;
}

const WebKitHTMLEntityTableEntry* WebKitHTMLEntitySearch::findFirst(u16 nextCharacter) const
{
    auto span = m_entries;
    if (span.size() == 1)
        return &span.first();
    CompareResult result = compare(&span.first(), nextCharacter);
    if (result == Prefix)
        return &span.first();
    if (result == After)
        return &span.last();
    while (span.size() > 2) {
        size_t mid = span.size() / 2;
        auto* probe = &span[mid];
        result = compare(probe, nextCharacter);
        if (result == Before)
            span = span.slice(probe - span.data());
        else {
            ASSERT(result == After || result == Prefix);
            span = span.slice(0, probe - span.data() + 1);
        }
    }
    ASSERT(span.size() == 2);
    return &span.last();
}

const WebKitHTMLEntityTableEntry* WebKitHTMLEntitySearch::findLast(u16 nextCharacter) const
{
    auto span = m_entries;
    if (span.size() == 1)
        return &span.last();
    CompareResult result = compare(&span.last(), nextCharacter);
    if (result == Prefix)
        return &span.last();
    if (result == Before)
        return &span.first();
    while (span.size() > 2) {
        size_t mid = span.size() / 2;
        auto* probe = &span[mid];
        result = compare(probe, nextCharacter);
        if (result == After)
            span = span.slice(0, probe - span.data() + 1);
        else {
            ASSERT(result == Before || result == Prefix);
            span = span.slice(probe - span.data());
        }
    }
    ASSERT(span.size() == 2);
    return &span.first();
}

void WebKitHTMLEntitySearch::advance(u16 nextCharacter)
{
    ASSERT(isEntityPrefix());
    if (!m_currentLength) {
        m_entries = WebKitHTMLEntityTable::entriesStartingWith(nextCharacter);
        if (m_entries.is_empty())
            return;
    } else {
        auto* first = findFirst(nextCharacter);
        m_entries = m_entries.slice(first - m_entries.data());
        auto* last = findLast(nextCharacter);
        m_entries = m_entries.slice(0, last - m_entries.data() + 1);
        if (first == last && compare(first, nextCharacter) != Prefix)
            return fail();
    }
    ++m_currentLength;
    if (m_entries[0].nameLength() != m_currentLength)
        return;
    m_mostRecentMatch = &m_entries.first();
}

}
