#pragma once

#include <Processors/QueryPlan/IQueryPlanStep.h>
#include <Processors/QueryPlan/ITransformingStep.h>

namespace DB
{

class IJoin;
using JoinPtr = std::shared_ptr<IJoin>;

class FullSortingMergeJoin;
class SortingStep;

/// Step for JOIN.
class JoinStep : public IQueryPlanStep
{
public:
    JoinStep(
        const DataStream & left_stream_,
        const DataStream & right_stream_,
        JoinPtr join_,
        size_t max_block_size_,
        size_t max_streams_,
        bool keep_left_read_in_order_);

    String getName() const override { return "Join"; }

    QueryPipelineBuilderPtr updatePipeline(QueryPipelineBuilders pipelines, const BuildQueryPipelineSettings &) override;

    void describePipeline(FormatSettings & settings) const override;

    const JoinPtr & getJoin() const { return join; }
    bool allowPushDownToRight() const;

    void updateInputStream(const DataStream & new_input_stream_, size_t idx);

private:
    FullSortingMergeJoin * getSortingJoin();

    JoinPtr join;
    size_t max_block_size;
    size_t max_streams;
    bool keep_left_read_in_order;

    std::unique_ptr<SortingStep> left_sorting;
    std::unique_ptr<SortingStep> right_sorting;
};

/// Special step for the case when Join is already filled.
/// For StorageJoin and Dictionary.
class FilledJoinStep : public ITransformingStep
{
public:
    FilledJoinStep(const DataStream & input_stream_, JoinPtr join_, size_t max_block_size_);

    String getName() const override { return "FilledJoin"; }
    void transformPipeline(QueryPipelineBuilder & pipeline, const BuildQueryPipelineSettings &) override;

private:
    void updateOutputStream() override;

    JoinPtr join;
    size_t max_block_size;
};

}
