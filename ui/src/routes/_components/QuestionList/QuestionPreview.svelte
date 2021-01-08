<script>
	import * as api from 'api.js';

	export let question;
	export let user;

	async function toggleFavorite() {
		// optimistic UI
		if (question.favorited) {
			question.favoritesCount -= 1;
			question.favorited = false;
		} else {
			question.favoritesCount += 1;
			question.favorited = true;
		}

		({ question } = await (question.favorited
			? api.post(`questions/${question.slug}/favorite`, null, user && user.token)
			: api.del(`questions/${question.slug}/favorite`, user && user.token)));
	}
</script>

<div class="question-preview">
	<div class="question-meta">
		<a href='/profile/@{question.author.username}'>
			<img src={question.author.image} alt={question.author.username} />
		</a>

		<div class="info">
			<a class="author" href='/profile/@{question.author.username}'> {question.author.username}
			</a>
			<span class="date">
				{new Date(question.createdAt).toDateString()}
			</span>
		</div>

		{#if user}
			<div class="pull-xs-right">
				<button class='btn btn-sm {question.favorited ? "btn-primary" : "btn-outline-primary"}' on:click={toggleFavorite}>
					<i class="ion-heart"></i> {question.favoritesCount}
				</button>
			</div>
		{/if}
	</div>

	<a href='/question/{question.slug}' rel='prefetch' class="preview-link">
		<h1>{question.title}</h1>
		<p>{question.description}</p>
		<span>Read more...</span>
		<ul class="tag-list">
			{#each question.tagList as tag}
				<li class="tag-default tag-pill tag-outline">
					{tag}
				</li>
			{/each}
		</ul>
	</a>
</div>