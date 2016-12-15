/*
 * Copyright (c) 2015, The University of Oxford
 * All rights reserved.
 *
 * This file is part of the OSKAR package.
 * Contact: oskar at oerc.ox.ac.uk
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the University of Oxford nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "oskar_SettingsNode.hpp"

using namespace std;

namespace oskar {

SettingsNode::SettingsNode()
: SettingsItem(),
  parent_(0),
  children_(vector<SettingsNode*>()),
  value_set_counter_(0)
{
}

SettingsNode::SettingsNode(const SettingsNode& node, SettingsNode* parent)
: SettingsItem(node),
  parent_(parent),
  children_(node.children_),
  value_set_counter_(0)
{
}

SettingsNode::SettingsNode(const string& key,
             const string& label,
             const string& description,
             const string& type_name,
             const string& type_default,
             const string& type_parameters,
             bool is_required,
             int priority)
: SettingsItem(key, label, description, type_name, type_default,
        type_parameters, is_required, priority),
  parent_(0),
  children_(vector<SettingsNode*>()),
  value_set_counter_(0)
{
}

SettingsNode::~SettingsNode()
{
    for (unsigned i = 0; i < children_.size(); ++i)
        delete children_.at(i);
}

bool SettingsNode::value_or_child_set() const
{
    return value_set_counter_ > 0;
}

int SettingsNode::num_children() const
{
    return children_.size();
}

SettingsNode* SettingsNode::add_child(const SettingsNode& node)
{
    if (node.item_type() != SettingsItem::INVALID) {
        children_.push_back(new SettingsNode(node, this));
        update_priority(node.priority());
        return children_.back();
    }
    else {
        return 0;
    }
}

SettingsNode* SettingsNode::child(int i)
{
    return children_.at(i);
}

const SettingsNode* SettingsNode::child(int i) const
{
    return children_.at(i);
}


SettingsNode* SettingsNode::child(const std::string& key)
{
    for (unsigned i = 0; i < children_.size(); ++i)
        if (children_.at(i)->key() == key) return children_.at(i);
    return 0;
}

const SettingsNode* SettingsNode::child(const std::string& key) const
{
    for (unsigned i = 0; i < children_.size(); ++i)
        if (children_.at(i)->key() == key) return children_.at(i);
    return 0;
}

const SettingsNode* SettingsNode::parent() const
{
    return parent_;
}

int SettingsNode::child_number() const
{
    if (parent_)
    {
        for (unsigned i = 0; i < children_.size(); ++i)
            if (children_.at(i) == this) return i;
    }
    return 0;
}

bool SettingsNode::set_value(const std::string& v)
{
    bool was_set = value().is_set();
    bool ok = SettingsItem::set_value(v);
    bool is_set = value().is_set();
    // Only update the counter if the set state has changed. ie. changed from
    // default to non default.
    if (was_set != is_set)
        update_value_set_counter_(is_set);
    return ok;
}

void SettingsNode::update_value_set_counter_(bool increment_counter)
{
    if (increment_counter)
        ++value_set_counter_;
    else
        --value_set_counter_;

    // Recursively set the counter for parents.
    if (parent_)
        parent_->update_value_set_counter_(increment_counter);
}

void SettingsNode::update_priority(int priority)
{
    this->priority_ += priority;
    if (parent_)
        parent_->update_priority(priority);
}

} // namespace oskar
