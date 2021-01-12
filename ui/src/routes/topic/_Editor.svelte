<script>
	import { goto, stores } from "@sapper/app";
	import * as api from "api.js";
	import { onMount } from "svelte";
	import "bytemd/dist/index.min.css";
	// import gfm from "@bytemd/plugin-gfm";

	export let topic;
	export let slug;

	let inProgress = false;
	let errors;

	const { session } = stores();

	function addTag(input) {
		topic.tagList = [...topic.tagList, input.value];
		input.value = "";
	}

	function remove(index) {
		topic.tagList = [
			...topic.tagList.slice(0, index),
			...topic.tagList.slice(index + 1),
		];
	}

	async function publish() {
		inProgress = true;

		const response = await (slug
			? api.put(
					`topics/${slug}`,
					{ topic },
					$session.user && $session.user.token
			  )
			: api.post(
					"topics",
					{ topic },
					$session.user && $session.user.token
			  ));

		if (response.topic) {
			goto(`/topic/${response.topic.slug}`);
		}

		inProgress = false;
	}

	function enter(node, callback) {
		function onkeydown(event) {
			if (event.which === 13) callback(node);
		}

		node.addEventListener("keydown", onkeydown);

		return {
			destroy() {
				node.removeEventListener("keydown", onkeydown);
			},
		};
	}

	let Editor = null;
	let plugins = null;
	let value = "";
	let Tags = null;

	onMount(async () => {
		const bytemd = await import("bytemd");
		Tags = (await import("svelte-tags-input")).default;
		Editor = bytemd.Editor;
	});

	function handleChange(e) {
		value = e.detail.value;
	}

	// If on:tags is defined
	let tags = "";
	const countryList = [
		"Afghanistan",
		"Albania",
		"Algeria",
		"American Samoa",
		"Andorra",
		"Angola",
		"Anguilla",
		"Antarctica",
		"Antigua and Barbuda",
		"Argentina",
	];
	function handleTags(event) {
		tags = event.detail.tags;
	}
</script>

<style>
	.wrapper {
		width: 100%;
		height: 100px;
		display: flex;
		align-items: center;
		justify-content: center;
	}
</style>

<div class="editor-page">
	<div class="container page">
		<div class="row">
			<div class="col-md-10 offset-md-1 col-xs-12">
				<h3>Post your topic for discussion</h3>
				<hr />
				<form>
					<fieldset>
						<fieldset class="form-group">
							<label
								for="title"
								class="form-label"
								style="float:left;margin-right: 10px;margin-top: 8px">Title:</label>
							<span style="display: block;overflow: hidden;">
								<input
									class="form-control form-control"
									style="width:100%;"
									name="title"
									type="text"
									placeholder="Topic summary"
									bind:value={topic.title}
									required=""
									minlength="10"
									maxlength="256" />
							</span>
						</fieldset>

						<fieldset class="form-group" style="margin-top:20px">
							<svelte:component
								this={Editor}
								on:change={handleChange}
								{value} />
						</fieldset>
						<fieldset class="form-group" style="margin-top:20px">
							<label
								for="tags"
								class="form-label"
								style="float:left;margin-right: 10px;margin-top: 8px">Tags:</label>
							<svelte:component
								this={Tags}
								name={"tags"}
								on:tags={handleTags}
								addKeys={[9]}
								maxTags={5}
								allowPaste={true}
								allowDrop={true}
								splitWith={','}
								onlyUnique={true}
								removeKeys={[27]}
								placeholder={'Tags, tab to complete'}
								allowBlur={true}
								disable={false}
								autocomplete={countryList}
								minChars={3} />
						</fieldset>
						<div class="wrapper">
							<button class="btn btn btn-primary pull-xs-right">
								Create Topic
							</button>
						</div>
					</fieldset>
				</form>
			</div>
		</div>
	</div>
</div>
