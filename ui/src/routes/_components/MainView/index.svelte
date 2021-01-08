<script>
	import { stores } from '@sapper/app';
	import QuestionList from '../QuestionList/index.svelte';

	export let tab = 'all';
	export let tag = null;
	export let p;

	const { session } = stores();

	function questions() {
		tab = "all";
		tag = null;
	}
	function popular() {
		tab="popular";
		tag = null;
	}
</script>

<div class="col-md-9">
	<div class="feed-toggle">
		<ul class="nav nav-tabs">
			<li class="nav-item">
				<a href="." class='nav-link {tab === "all" ? "active" : "" }' on:click='{questions}'>
					Qustions
				</a>
			</li>
			<li class="nav-item">
				<a href="." class='nav-link {tab === "popular" ? "active" : "" }' on:click='{popular}'>
					Popular
				</a>
			</li>

			{#if tag}
				<li class="nav-item">
					<a href="." class='nav-link {tab === "tag" ? "active" : "" }' on:click='{() => tab = "tag"}'>
						<i class="ion-pound"></i> {tag}
					</a>
				</li>
			{/if}
		</ul>
	</div>

	<QuestionList {p} {tab} {tag}/>
</div>