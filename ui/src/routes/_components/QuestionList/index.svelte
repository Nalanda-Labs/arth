<script>
	import { stores } from '@sapper/app';
	import QuestionPreview from './QuestionPreview.svelte';
	import ListPagination from './ListPagination.svelte';
	import * as api from 'api.js';

	export let tab, username = false;
	export let favorites = false;
	export let tag;
	export let p;

	const { session, page } = stores();

	let query;
	let questions;
	let questionsCount;

	$: {
		const endpoint = tab === 'feed' ? 'questions/feed' : 'questions';
		const page_size = tab === 'feed' ? 5 : 10;

		let params = `limit=${page_size}&offset=${(p - 1) * page_size}`;
		if (tab === 'tag') params += `&tag=${tag}`;
		if (tab === 'profile') params += `&${favorites ? 'favorited' : 'author'}=${encodeURIComponent(username)}`;

		query = `${endpoint}?${params}`;
	}

	$: query && getData();

	async function getData() {
		questions = null;

		// TODO do we need some error handling here?
		({ questions, questionsCount } = await api.get(query, $session.user && $session.user.token));
	}
</script>

{#if questions}
	{#if questions.length === 0}
		<div class="question-preview">
			No questions are here... yet.
		</div>
	{:else}
		<div>
			{#each questions as question (question.slug)}
				<QuestionPreview {question} user={$session.user}/>
			{/each}

			<ListPagination {questionsCount} page={parseInt($page.params.user, 10)}  />
		</div>
	{/if}
{:else}
	<div class="question-preview">Loading...</div>
{/if}